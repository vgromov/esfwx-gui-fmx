unit EsWinHTMLHelpViewer;

interface

{$IFDEF MSWINDOWS}

uses System.Types, FMX.Controls, FMX.Forms;

function SetHTMLHelpFile(PathAndFilename: string): boolean;
function ShowHTMLHelpContents : boolean;
function ShowHTMLHelp : boolean;
function ShowFocusedFormHTMLHelp(AForm: TCommonCustomForm) : boolean;
function ShowControlHTMLHelp(aControl: TStyledControl) : boolean;

function ShowPopupHTMLHelp : boolean;
function ShowFocusedFormPopupHTMLHelp(AForm: TCommonCustomForm) : boolean;
function ShowControlPopupHTMLHelp(aControl: TStyledControl) : boolean;

procedure RegisterFormForHelp(AForm: TCommonCustomForm);
{$ENDIF}

implementation

{$IFDEF MSWINDOWS}

uses Winapi.Windows, Winapi.Messages, System.Classes, FMX.Types,
  FMX.Platform.Win,
  System.SysUtils;

type
  TFMXHtmlHelpViewer = class(TInterfacedObject)
  private
    FInitializedCookie: DWORD;
    FInitialized: boolean;
    FHelpFile: String;
    FHelpHandle: THandle;
    procedure SetHelpFile(const Value: String);
    function GetHelpHandle: THandle;
    procedure SetHelpHandle(AHandle: THandle);
    function DisplayTextPopupData(Position: TPoint; ResInstance: HINST;
      AHelpString: string; HelpStringResId: UINT) : boolean;

  protected
    function DisplayTopic(const Topic: String) : boolean;
    function ShowHelp(const HelpString: String) : boolean;
    function ShowTableOfContents : boolean;
    procedure ShutDown;
    procedure SoftShutDown;
    procedure ValidateHelpViewer;
    function DisplayTextPopup(Data: PHH_Popup) : boolean;
    function DisplayHTMLTextPopup(Data: PHH_Popup) : boolean;
    function DisplayTextPopupStr(AHelpString: string) : boolean;
    function DisplayTextPopupRes(HelpStringResId: UINT) : boolean;
    function DisplayTextPopupStrPos(Position: TPoint; AHelpString: string) : boolean;
    function DisplayTextPopupResPos(Position: TPoint; HelpStringResId: UINT) : boolean;

    function LookupALink(LinkPtr: PHH_AKLINK) : boolean;
    function LookupKeyWord(LinkPtr: PHH_AKLINK) : boolean;
    function SynchTopic : boolean;
    property HelpHandle: THandle read GetHelpHandle write SetHelpHandle;
  public
    function HelpContext(Context: THelpContext): boolean;
    function HelpJump(const JumpID: string): boolean;
    function HelpKeyword(const Keyword: string): boolean;
    function HelpShowTableOfContents: boolean;
    function DoHelp : boolean;
    function DoHelpOnForm(AForm: TCommonCustomForm) : boolean;
    function DoHelpOnControl(aControl: TStyledControl) : boolean;
    function DoPopupHelp : boolean;
    function DoPopupHelpOnForm(AForm: TCommonCustomForm) : boolean;
    function DoPopupHelpOnControl(aControl: TStyledControl) : boolean;

    property HelpFile: String read FHelpFile write SetHelpFile;
  end;

  TWndProc = function(hwnd: hwnd; uMsg: UINT; wParam: wParam; lParam: lParam)
    : LRESULT; stdcall;

var
  FMXHtmlHelpViewer: TFMXHtmlHelpViewer = nil;
  OldFormWndProc: TWndProc = nil;

function HelpWndProc(hwnd: hwnd; uMsg: UINT; wParam: wParam; lParam: lParam)
  : LRESULT; stdcall;
begin

  // unfortunately FMX does not pass the WM_HELP from the chils control
  // to parent when WS_EX_CONTEXTHELP is used. (the point is moot though
  // as FMX ignores biHelp)
  if (uMsg = WM_HELP) then
    if GetKeyState(VK_CONTROL) < 0 then
      ShowPopupHTMLHelp
    else
      ShowHTMLHelp;

  if @OldFormWndProc <> nil then
    Result := OldFormWndProc(hwnd, uMsg, wParam, lParam)
  else
    Result := 0; // pacify the compiler
end;

procedure RegisterFormForHelp(AForm: TCommonCustomForm);
var
  FormWndProc: NativeInt;
begin
  FormWndProc := GetWindowLong(
    WindowHandleToPlatform(AForm.Handle).Wnd,
    GWL_WNDPROC
  );

  if @OldFormWndProc = nil then
    @OldFormWndProc := Pointer(FormWndProc);

  if FormWndProc = NativeInt(@OldFormWndProc) then
    SetWindowLong(
      WindowHandleToPlatform(AForm.Handle).Wnd,
      GWL_WNDPROC,
      NativeInt(@HelpWndProc)
    );
end;

function SetHTMLHelpFile(PathAndFilename: string): boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FileExists(PathAndFilename);
  if Result then
    FMXHtmlHelpViewer.HelpFile := PathAndFilename;
end;

function ShowHTMLHelp : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoHelp;
end;

function ShowFocusedFormHTMLHelp(AForm: TCommonCustomForm) : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoHelpOnForm(AForm);
end;

function ShowControlHTMLHelp(aControl: TStyledControl) : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoHelpOnControl(aControl);
end;

function ShowHTMLHelpContents : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.ShowTableOfContents;
end;

function ShowPopupHTMLHelp : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoPopupHelp;
end;

function ShowFocusedFormPopupHTMLHelp(AForm: TCommonCustomForm) : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoPopupHelpOnForm(AForm);
end;

function ShowControlPopupHTMLHelp(aControl: TStyledControl) : boolean;
begin
  Result := false;

  if FMXHtmlHelpViewer <> nil then
    Result := FMXHtmlHelpViewer.DoPopupHelpOnControl(aControl);
end;

{ TFMXHtmlHelpViewer }

procedure TFMXHtmlHelpViewer.ValidateHelpViewer;
begin
  if not FInitialized then
  begin
    HtmlHelp(0, nil, HH_INITIALIZE, &FInitializedCookie);
    FInitialized := true;
  end;
end;

function TFMXHtmlHelpViewer.ShowTableOfContents : boolean;
begin
  ValidateHelpViewer;
  Result := SynchTopic AND (HtmlHelp(HelpHandle, PChar(FHelpFile), HH_DISPLAY_TOC, 0) <> 0);
end;

procedure TFMXHtmlHelpViewer.SetHelpFile(const Value: String);
begin
  FHelpFile := Value;
end;

procedure TFMXHtmlHelpViewer.SetHelpHandle(AHandle: THandle);
begin
  if AHandle <> FHelpHandle then
    FHelpHandle := AHandle;
end;

function TFMXHtmlHelpViewer.GetHelpHandle: THandle;
var
  FocusedForm: TCommonCustomForm;
begin
  if IsWindow(FHelpHandle) then
    Result := FHelpHandle
  else
  begin
    FocusedForm := Screen.ActiveForm;
    if FocusedForm <> nil then
      Result := WindowHandleToPlatform(FocusedForm.Handle).Wnd
    else
      Result := Winapi.Windows.GetDesktopWindow;
  end;
end;

function TFMXHtmlHelpViewer.HelpContext(Context: THelpContext): boolean;
begin
  ValidateHelpViewer;
  Result := SynchTopic AND (HtmlHelp(HelpHandle, PChar(FHelpFile), HH_HELP_CONTEXT, Context) <> 0);
end;

function TFMXHtmlHelpViewer.HelpJump(const JumpID: string): boolean;
begin
  Result := ShowHelp(JumpID);
end;

function TFMXHtmlHelpViewer.HelpKeyword(const Keyword: string): boolean;
begin
  Result := ShowHelp(Keyword);
end;

function TFMXHtmlHelpViewer.HelpShowTableOfContents: boolean;
begin
  Result := ShowTableOfContents;
end;

function TFMXHtmlHelpViewer.ShowHelp(const HelpString: String) : boolean;
var
  Link: THH_AKLINK;
begin
  ValidateHelpViewer;
  Result := false;

  if SynchTopic then
  begin
    Link.cbStruct := sizeof(THH_AKLINK);
    Link.fReserved := false;
    Link.pszKeywords := PChar(HelpString);
    Link.pszUrl := nil;
    Link.pszMsgText := nil;
    Link.pszMsgTitle := nil;
    Link.pszWindow := nil;
    Link.fIndexOnFail := true;
    Result := LookupKeyWord(@Link);
  end;
end;

{ SoftShutDown is called by the help manager to ask the viewer to
  terminate any externally spawned subsystem without shutting itself down. }
procedure TFMXHtmlHelpViewer.SoftShutDown;
begin
  HtmlHelp(0, nil, HH_CLOSE_ALL, 0);
end;

procedure TFMXHtmlHelpViewer.ShutDown;
begin
  SoftShutDown;
  if FInitialized then
  begin
    HtmlHelp(0, nil, HH_UNINITIALIZE, &FInitializedCookie);
    FInitialized := false;
    FInitializedCookie := 0;
  end;
end;

function TFMXHtmlHelpViewer.DisplayTopic(const Topic: String) : boolean;
const
  InvokeSep = '::/';
  InvokeSuf = '.htm';
var
  HelpFile: String;
  InvocationString: String;
begin
  ValidateHelpViewer;
  InvocationString := HelpFile + InvokeSep + Topic + InvokeSuf;
  Result := HtmlHelp(HelpHandle, PChar(InvocationString), HH_DISPLAY_TOPIC, 0) <> 0;
end;

function TFMXHtmlHelpViewer.DoHelpOnControl(aControl: TStyledControl) : boolean;
begin
  Result := false;

  if aControl <> nil then
  begin
    if aControl.Root is TCommonCustomForm then
      HelpHandle := WindowHandleToPlatform(TCommonCustomForm(aControl.Root).Handle).Wnd;

    if aControl.HelpType = THelpType.htKeyword then
      Result := HelpKeyword(aControl.HelpKeyword)
    else
      Result := HelpContext(aControl.HelpContext);
  end;
end;

function TFMXHtmlHelpViewer.DoHelpOnForm(AForm: TCommonCustomForm) : boolean;
begin
  Result := false;

  if AForm <> nil then
    if AForm.Focused <> nil then
      if AForm.Focused.GetObject is TStyledControl then
        Result := DoHelpOnControl(TStyledControl(AForm.Focused.GetObject));
end;

function TFMXHtmlHelpViewer.DoPopupHelpOnControl(aControl: TStyledControl) : boolean;
var
  BottomLeft: TPointF;
  CallingForm: TCommonCustomForm;
begin
  Result := false;
  CallingForm := nil;

  if aControl <> nil then
  begin
    if aControl.Root is TCommonCustomForm then
      CallingForm := TCommonCustomForm(aControl.Root);
    if CallingForm <> nil then
    begin
      HelpHandle := WindowHandleToPlatform(CallingForm.Handle).Wnd;
      BottomLeft := CallingForm.ClientToScreen
        (PointF(aControl.BoundsRect.Left, aControl.BoundsRect.Bottom));
      if aControl.HelpType = THelpType.htKeyword then
        Result := DisplayTextPopupStrPos(BottomLeft.Round, aControl.HelpKeyword)
      else
        Result := DisplayTextPopupResPos(BottomLeft.Round, aControl.HelpContext);
    end;
  end;
end;

function TFMXHtmlHelpViewer.DoPopupHelpOnForm(AForm: TCommonCustomForm) : boolean;
begin
  Result := false;

  if AForm <> nil then
    if AForm.Hovered <> nil then
      if AForm.Hovered.GetObject is TStyledControl then
        Result := DoPopupHelpOnControl(TStyledControl(AForm.Hovered.GetObject));
end;

function TFMXHtmlHelpViewer.DoPopupHelp : boolean;
begin
  Result := DoPopupHelpOnForm(Screen.ActiveForm);
end;

function TFMXHtmlHelpViewer.DoHelp : boolean;
begin
  Result := DoHelpOnForm(Screen.ActiveForm);
end;

function TFMXHtmlHelpViewer.SynchTopic : boolean;
begin
  Result := HtmlHelp(HelpHandle, PChar(FHelpFile), HH_DISPLAY_TOC, 0) <> 0;
end;

function TFMXHtmlHelpViewer.LookupALink(LinkPtr: PHH_AKLINK) : boolean;
begin
  Result := HtmlHelp(HelpHandle, PChar(FHelpFile), HH_ALINK_LOOKUP, DWORD_PTR(LinkPtr)) <> 0;
end;

function TFMXHtmlHelpViewer.LookupKeyWord(LinkPtr: PHH_AKLINK) : boolean;
begin
  Result := HtmlHelp(HelpHandle, PChar(FHelpFile), HH_KEYWORD_LOOKUP, DWORD_PTR(LinkPtr)) <> 0;
end;

function TFMXHtmlHelpViewer.DisplayHTMLTextPopup(Data: PHH_Popup) : boolean;
begin
  Result := HtmlHelp(HelpHandle, PChar(FHelpFile), HH_DISPLAY_TEXT_POPUP,
    DWORD_PTR(Data)) <> 0;
end;

function TFMXHtmlHelpViewer.DisplayTextPopup(Data: PHH_Popup) : boolean;
begin
  Result := HtmlHelp(HelpHandle, nil, HH_DISPLAY_TEXT_POPUP, DWORD_PTR(Data)) <> 0;
end;

function TFMXHtmlHelpViewer.DisplayTextPopupData(Position: TPoint;
  ResInstance: HINST; AHelpString: string; HelpStringResId: UINT) : boolean;
var
  PopupData: HH_Popup;
begin
  PopupData.cbStruct := sizeof(HH_Popup);
  PopupData.HINST := ResInstance;
  PopupData.idString := HelpStringResId;
  PopupData.pszText := PChar(AHelpString);
  PopupData.pt := Position;
  PopupData.clrForeground := TColorRef(-1);
  PopupData.clrBackground := TColorRef(-1);
  PopupData.rcMargins.Left := -1;
  PopupData.rcMargins.Right := -1;
  PopupData.rcMargins.Top := -1;
  PopupData.rcMargins.Bottom := -1;
  PopupData.pszFont := nil;
  Result := DisplayTextPopup(@PopupData);
end;

function TFMXHtmlHelpViewer.DisplayTextPopupRes(HelpStringResId: UINT) : boolean;
begin
  Result := false;
end;

function TFMXHtmlHelpViewer.DisplayTextPopupResPos(Position: TPoint;
  HelpStringResId: UINT) : boolean;
begin
  // code can be improved to specify a resource module for the strings
  Result := DisplayTextPopupData(Position, GetModuleHandle(nil), '', HelpStringResId);
end;

function TFMXHtmlHelpViewer.DisplayTextPopupStr(AHelpString: string) : boolean;
begin
  Result := false;
end;

function TFMXHtmlHelpViewer.DisplayTextPopupStrPos(Position: TPoint;
  AHelpString: string) : boolean;
begin
  Result := DisplayTextPopupData(Position, 0, AHelpString, 0);
end;

initialization

FMXHtmlHelpViewer := TFMXHtmlHelpViewer.Create;

finalization

if FMXHtmlHelpViewer <> nil then
begin
  FMXHtmlHelpViewer.ShutDown;
  FMXHtmlHelpViewer := nil;
end;

{$ENDIF}

end.
