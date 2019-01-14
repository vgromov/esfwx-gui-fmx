#include "escore_gui_pch.h"
#pragma hdrstop

#include "TDlgBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

__fastcall TDlgBase::TDlgBase(TComponent* Owner) :
TForm(Owner),
m_i18nScope(this),
m_minW(0),
m_minH(0)
{
  layBtns_->ShowHint = true;
  layContents_->ShowHint = true;
}
//---------------------------------------------------------------------------

__fastcall TDlgBase::~TDlgBase()
{
}
//---------------------------------------------------------------------------

void TDlgBase::i18nInit()
{
  m_i18nScope.onGuiStringsUpdate = onI18nStringsUpdate;
	m_i18nScope.managerSet( EsI18nManager::instanceGet() );
}
//---------------------------------------------------------------------------

void __fastcall TDlgBase::onI18nStringsUpdate(TObject* sender)
{}
//---------------------------------------------------------------------------

void TDlgBase::payloadSet(const EsVariant& payload)
{
  m_payload = payload;
}
//---------------------------------------------------------------------------

const EsVariant& TDlgBase::payloadGet() const
{
  return m_payload;
}
//---------------------------------------------------------------------------

void TDlgBase::formReposition()
{
	TCustomForm* mainFrm = dynamic_cast<TCustomForm*>(Application->MainForm);
	TRectF bounds;
	bool positioned = false;
	if( mainFrm )
	{
		bounds.Top = mainFrm->Top;
		bounds.Left = mainFrm->Left;
		bounds.SetWidth( mainFrm->Width );
		bounds.SetHeight( mainFrm->Height );
		positioned = true;
	}
	else
	{
		_di_IInterface ii = TPlatformServices::Current->GetPlatformService(__uuidof(IFMXScreenService));
		if( ii )
		{
			_di_IFMXScreenService ss = ii;
			if( ss )
			{
				const TPointF& size = ss->GetScreenSize();
				bounds.SetWidth(size.X);
				bounds.SetHeight(size.Y);
				positioned = true;
			}
		}
	}

	if( positioned )
	{
		SetBounds(
			(bounds.GetWidth() - Width) / 2 + bounds.Left,
			(bounds.GetHeight() - Height) / 2 + bounds.Top,
			Width,
			Height);
	}
}
//---------------------------------------------------------------------------

void TDlgBase::onDlgShow()
{
	// position in the center of the main form, otherwise, the screen
  formReposition();
}
//---------------------------------------------------------------------------

void __fastcall TDlgBase::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = TCloseAction::caFree;
}
//---------------------------------------------------------------------------

void __fastcall TDlgBase::FormShow(TObject *Sender)
{
  onDlgShow();
}
//---------------------------------------------------------------------------

void TDlgBase::restrictMinWidthHeight()
{
  float newH = ClientHeight;
  float newW = ClientWidth;

  if( newH < static_cast<float>(m_minH) )
    newH = static_cast<float>(m_minH);

  if( newW < static_cast<float>(m_minW) )
    newW = static_cast<float>(m_minW);

  TNotifyEvent onresize = OnResize;
  OnResize = nullptr;

  if( !EsUtilities::areEqualFloats( ClientHeight, newH, 4 ) )
    ClientHeight = newH;

  if( !EsUtilities::areEqualFloats( ClientWidth, newW, 4) )
    ClientWidth = newW;

  OnResize = onresize;
}
//---------------------------------------------------------------------------

void TDlgBase::setMinWidthHeight(ulong w, ulong h)
{
  if( 0 < w && w < 320 )
    w = 320;

  if( 0 < h && h < 240 )
    h = 240;

  m_minH = h;
  m_minW = w;

  restrictMinWidthHeight();
}
//---------------------------------------------------------------------------

void TDlgBase::onResize()
{
  restrictMinWidthHeight();
}
//---------------------------------------------------------------------------

#ifdef ES_IS_MOBILE_OS
class TMethodReference : public TCppInterfacedObject< TProc__1<TModalResult> >
{
public:
  void __fastcall Invoke(TModalResult mr)
  {
    if( m_onClose )
      m_onClose(m_dlg, mr, m_dlg->payloadGet());

    if( m_dlg->Parent )
      m_dlg->Parent = 0;
    if( m_dlg->Owner )
      m_dlg->Owner->RemoveComponent(m_dlg);

    EsUtilities::actionClientUnregisterFix(m_dlg);
    m_dlg->Release();
    m_dlg = 0;
  }

  TDlgBase *m_dlg;
  EsDlgOnClose m_onClose;
};
#endif
//---------------------------------------------------------------------------

void TDlgBase::showModal(EsDlgOnClose onClose)
{
#ifdef ES_IS_MOBILE_OS
   TMethodReference* pfn = new TMethodReference;
   ES_ASSERT(pfn);
   pfn->m_dlg = this;
   pfn->m_onClose = onClose;
   i18nInit();
   ShowModal(pfn);
#else
  i18nInit();
  TModalResult mr = ShowModal();
  if( onClose )
    onClose(this, mr, m_payload);
#endif
}
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP
void TDlgBase::showModal( std::function<void (TDlgBase* Sender, TModalResult mr, const EsVariant& payload)> onClose )
{
  i18nInit();
  TModalResult mr = ShowModal();
  if( onClose )
    onClose(this, mr, m_payload);
}
#endif
//---------------------------------------------------------------------------

void __fastcall TDlgBase::FormResize(TObject *Sender)
{
  onResize();
}
//---------------------------------------------------------------------------

void __fastcall TDlgBase::btn_ApplyStyleLookup(TObject *Sender)
{
  TButton* btn = dynamic_cast<TButton*>(Sender);
  if( !btn )
    return;

  const TRectF& ext = EsUtilities::textRectGet(
    btn,
    btn->Text.c_str()
  );

  float newW = ext.Width()+20;
  if( btn->Width < newW )
    btn->Width = newW;
}
//---------------------------------------------------------------------------

