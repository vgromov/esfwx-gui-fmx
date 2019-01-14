#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsCoreGuiUtilities.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------

EsSingleInstanceGuard::EsSingleInstanceGuard(const UnicodeString& name) :
m_name(name),
#if ES_OS == ES_OS_WINDOWS
m_mx(nullptr),
m_msgId(0),
#endif
m_ok(false),
m_duplicate(false)
{
#if ES_OS == ES_OS_WINDOWS
  SECURITY_DESCRIPTOR securityDesc;
  SECURITY_ATTRIBUTES securityAttr;

  memset( &securityDesc, sizeof(securityDesc), 0 );
  memset( &securityAttr, sizeof(securityAttr), 0 );

  ::InitializeSecurityDescriptor(
    &securityDesc,
    SECURITY_DESCRIPTOR_REVISION
  );

  ::SetSecurityDescriptorDacl(
    &securityDesc,
    TRUE,
    nullptr,
    FALSE
  );

  securityAttr.nLength = sizeof(securityAttr);
  securityAttr.lpSecurityDescriptor = &securityDesc;
  securityAttr.bInheritHandle = FALSE;

  m_mx = ::CreateMutex(
    &securityAttr,
    FALSE,
    m_name.c_str()
  );

  if( NULL == m_mx )
    return;

  if( ERROR_ALREADY_EXISTS == GetLastError() )
    m_duplicate = true;

  const UnicodeString& msgGuard = esT("SINGLE_APP_GUARD_") + m_name;
  m_msgId = ::RegisterWindowMessage(
    msgGuard.c_str()
  );
#else

#endif

  m_ok = true;
}
//---------------------------------------------------------------------------

EsSingleInstanceGuard::~EsSingleInstanceGuard()
{
#if ES_OS == ES_OS_WINDOWS
  if( m_mx )
  {
    ::CloseHandle(m_mx);
    m_mx = nullptr;
  }
#endif
}
//---------------------------------------------------------------------------

void EsSingleInstanceGuard::notifyInstance()
{
#if ES_OS == ES_OS_WINDOWS
  if( m_ok && m_msgId )
    ::SendMessage(
      HWND_BROADCAST,
      m_msgId,
      0,
      0
    );
#endif
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include <FMX.Platform.Win.hpp>

EsSingleInstanceGuardMsgHook* EsSingleInstanceGuardMsgHook::s_inst = nullptr;

EsSingleInstanceGuardMsgHook::EsSingleInstanceGuardMsgHook(UINT guardMsgId) :
m_hwnd( Fmx::Platform::Win::ApplicationHWND() ),
m_oldProc(NULL),
m_guardMsgId(guardMsgId),
m_installed(false)
{
  s_inst = this;
  m_oldProc = ::SetWindowLongPtr(
    m_hwnd,
    GWLP_WNDPROC,
    reinterpret_cast<LONG_PTR>(&HookWndProc)
  );

  m_installed = NULL != m_oldProc;
}
//---------------------------------------------------------------------------

EsSingleInstanceGuardMsgHook::~EsSingleInstanceGuardMsgHook()
{
  if( m_installed )
    ::SetWindowLongPtr(
      m_hwnd,
      GWLP_WNDPROC,
      m_oldProc
    );

  m_installed = false;
  m_hwnd = nullptr;
  m_oldProc = NULL;
  s_inst = nullptr;
}
//---------------------------------------------------------------------------

LRESULT CALLBACK EsSingleInstanceGuardMsgHook::HookWndProc(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
  if( s_inst &&
      uMsg == s_inst->m_guardMsgId &&
      hwnd == s_inst->m_hwnd
  )
  {
    if( Application->MainForm )
      Application->MainForm->Show();

    return 0;
  }

  return ::CallWindowProc(
    reinterpret_cast<WNDPROC>(s_inst->m_oldProc),
    hwnd,
    uMsg,
    wParam,
    lParam
  );
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Action list categorizer implementation
//
EsActionListCategorizer::EsActionListCategorizer( TContainedActionList* al /*= 0*/ )
{
	rebuild(al);
}
//---------------------------------------------------------------------------

void EsActionListCategorizer::rebuild( TContainedActionList* al )
{
	m_categories.clear();
	m_acts.clear();

	if( al )
	{
    TCustomActionList* cal = dynamic_cast<TCustomActionList*>(al);
    ES_ASSERT(cal);

    TCustomImageList* imgs = dynamic_cast<TCustomImageList*>(cal->Images);

		for( int idx = 0; idx < al->ActionCount; ++idx )
		{
			TContainedAction* act = al->Actions[idx];
			EsString category = act->Category.c_str();
			EsString::HashT hash = category.hashGet();

      act->ImageIndex =
        EsUtilities::imageIndexFindByName( imgs, act->Name );

			if( m_acts.end() == m_acts.find(hash) )
				m_categories.push_back(category);

			m_acts.insert( EsCategorizedActionsT::value_type(hash, act) );
		}
	}
}
//---------------------------------------------------------------------------

// get actions by category name
EsActionListCategorizer::EsActionsT EsActionListCategorizer::actionsGet(const EsString& category) const
{
	EsActionsT result;
	result.reserve(m_acts.size());

	std::pair<EsCategorizedActionsT::const_iterator, EsCategorizedActionsT::const_iterator> range =
		m_acts.equal_range(category.hashGet());

	for(EsCategorizedActionsT::const_iterator cit = range.first; cit != range.second; ++cit )
		result.push_back( (*cit).second );

	return result;
}
//---------------------------------------------------------------------------

// get all actions (ungrouped, but category-ordered access)
EsActionListCategorizer::EsActionsT EsActionListCategorizer::actionsGet() const
{
	EsActionsT result;
	result.reserve(m_acts.size());

	for(EsCategorizedActionsT::const_iterator cit = m_acts.begin(); cit != m_acts.end(); ++cit)
		result.push_back( (*cit).second );

	return result;
}
//---------------------------------------------------------------------------

/// Get action by its object name. If action is not found - return 0
TContainedAction* EsActionListCategorizer::actionGet(const EsString& name) const
{
	for(EsCategorizedActionsT::const_iterator cit = m_acts.begin(); cit != m_acts.end(); ++cit)
	{
		TContainedAction* act = (*cit).second;
		ES_ASSERT(act);
		if( act->Name == name.c_str() )
			return act;
	}

	return nullptr;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Utilities namespace impl
//

TObject* EsUtilities::NEW(TMetaClass* mc, const EsUtilities::EsArgsT& args)
{
	ES_ASSERT(mc);
  TRttiContext ctx;
  TRttiType* type = ctx.GetType(mc);
  if(type)
  {
    DynamicArray<TRttiMethod*> ctrs = type->GetMethods("Create");
    for( int idx = 0; idx < ctrs.Length; ++idx )
    {
      TRttiMethod* ctr = ctrs[idx];
      if( ctr->IsConstructor )
      {
        DynamicArray<TRttiParameter*> params = ctr->GetParameters();
        if( (DynArrInt)args.size() == params.Length )
        {
          int acnt = (int)args.size();
          const TValue* pargs = nullptr;
          if( !args.empty() )
            pargs = &args[0];

          return ctr->Invoke(mc, pargs, acnt-1).AsObject();
        }
      }
    }
  }

	return nullptr;
}
//---------------------------------------------------------------------------

static void propAssignVal(TRttiProperty* p, void* dest, const TValue& val)
{
  if( p )
    p->SetValue( dest, val );
}
//---------------------------------------------------------------------------

void EsUtilities::propertySet(TObject* obj, const EsString& prop, const TValue& val)
{
  ES_ASSERT(obj);

  TRttiContext ctx;
  TRttiType* type = ctx.GetType( obj->ClassType() );

  if(type)
  {
    TRttiProperty* rprop = type->GetProperty(prop.c_str());
    propAssignVal(rprop, obj, val);
  }
}
//---------------------------------------------------------------------------

TValue EsUtilities::propertyGet(TObject* obj, const EsString& prop)
{
  ES_ASSERT(obj);

  TRttiContext ctx;
  TRttiType* type = ctx.GetType( obj->ClassType() );
  if(type)
  {
    TRttiProperty* rprop = type->GetProperty(prop.c_str());
    if( rprop )
      return rprop->GetValue(obj);
  }

  return TValue::Empty;
}
//---------------------------------------------------------------------------

void EsUtilities::toStringList(TStrings* dest, const EsString::Array& src)
{
	ES_ASSERT(dest);
	dest->Clear();
	for(size_t idx = 0; idx < src.size(); ++idx)
		dest->Add( src[idx].c_str() );
}
//---------------------------------------------------------------------------

void EsUtilities::toStringListI18n(TStrings* dest, const EsString::Array& src)
{
	ES_ASSERT(dest);
	dest->Clear();
	for(size_t idx = 0; idx < src.size(); ++idx)
		dest->Add( EsStringI18n::translate(src[idx]).c_str() );
}
//---------------------------------------------------------------------------

void EsUtilities::fromStringList(EsString::Array& dest, const TStrings* src)
{
	ES_ASSERT(src);
	dest.clear();
	// mismathced constness workaround for FM3
	for(int idx = 0; idx < const_cast<TStrings*>(src)->Count; ++idx)
		dest.push_back( const_cast<TStrings*>(src)->Strings[idx].c_str() );
}
//---------------------------------------------------------------------------

TFmxObject* EsUtilities::childFindByName(TFmxObject* src, const UnicodeString& name)
{
  ES_ASSERT(src);
  for(int idx = 0; idx < src->ChildrenCount; ++idx)
  {
    TFmxObject* child = src->Children->Items[idx];
    if( name == child->Name )
      return child;
  }

  return nullptr;
}
//---------------------------------------------------------------------------

/// Forcibly update all actions in action list
///
void EsUtilities::actionListUpdate(TContainedActionList* al)
{
	if( al )
	{
		for( int idx = 0; idx < al->ActionCount; ++idx )
			al->Actions[idx]->Update();
	}
}
//---------------------------------------------------------------------------

/// Find action in action list by its component name
///
TContainedAction* EsUtilities::actionListFind(TContainedActionList* al, const UnicodeString& actionName)
{
	ES_ASSERT(al);
	for(int idx = 0; idx < al->ActionCount; ++idx)
	{
		TContainedAction* act = al->Actions[idx];
		ES_ASSERT(act);

		if( act->Name == actionName )
			return act;
	}

	return nullptr;
}
//---------------------------------------------------------------------------

// return extent of first-level contents, i.e. all immediate children
// of ctl container
TPointF EsUtilities::contentsExtentGet(TControl* ctl, int start /*= 0*/ )
{
	TPointF result(0, 0);

	if( !ctl || (ctl && !ctl->Visible) )
    return result;

  int cnt = ctl->ChildrenCount;

  if( !cnt )
  {
    result.X = ctl->Width+ctl->Margins->Left+ctl->Margins->Right;
    result.Y = ctl->Height+ctl->Margins->Top+ctl->Margins->Bottom;

    return result;
  }

  for( int idx = start; idx < cnt; ++idx )
  {
    TControl* child = dynamic_cast<TControl*>(ctl->Children->Items[idx]);
    if( !child || (child && !child->Visible) )
      continue;

    const TPointF& pos = child->Position->Point;
    const TPointF& ext = contentsExtentGet(child);

    if( result.X < pos.X+ext.X )
      result.X = pos.X+ext.X;

    if( result.Y < pos.Y+ext.Y )
      result.Y = pos.Y+ext.Y;
  }

	return result;
}
//---------------------------------------------------------------------------

/// Remove all components owned by the cmp
///
void EsUtilities::componentsRemoveFrom(TComponent* cmp)
{
	if( cmp )
	{
		for(int idx = 0; idx < cmp->ComponentCount; ++idx)
			delete cmp->Components[idx];
	}
}
//---------------------------------------------------------------------------

/// load form extent, state and position
///
void EsUtilities::formPosExtAndStateLoad(TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context)
{
	ES_ASSERT(form);
	ES_ASSERT(stream);

	EsStreamContextExistingScope scope(stream, context);
	if( scope.isOk() )
	{
    EsVariant val;

    int l = form->Left, t = form->Top, w = form->Width, h = form->Height;
		val =	stream->valueRead( esT("LEFT"), EsVariant::null() );
		if( !val.isEmpty() )
			l = val.asInt();
		val =	stream->valueRead( esT("TOP"), EsVariant::null() );
		if( !val.isEmpty() )
			t = val.asInt();

		val =	stream->valueRead( esT("WIDTH"), EsVariant::null() );
		if( !val.isEmpty() )
			w = val.asInt();

		val = stream->valueRead( esT("HEIGHT"), EsVariant::null() );
		if( !val.isEmpty() )
			h = val.asInt();

    const TPointF& ssze = screenSizeGet();
    if( l >= ssze.X - 50 )
      l = ssze.X - 50;
    if( t >= ssze.Y - 50 )
      t = ssze.Y - 50;

    form->SetBounds(l, t, w, h);

		val =	stream->valueRead( esT("STATE"), static_cast<int>(TWindowState::wsNormal) );
		TWindowState ws = static_cast<TWindowState>(val.asInt());
		form->WindowState = ws;
	}
}
//---------------------------------------------------------------------------

/// load form position
///
void EsUtilities::formPosLoad(TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context)
{
	ES_ASSERT(form);
	ES_ASSERT(stream);

	EsStreamContextExistingScope scope(stream, context);
	if( scope.isOk() )
	{
    const TPointF& ssze = screenSizeGet();

		EsVariant val =	stream->valueRead( esT("LEFT"), EsVariant::null() );
		if( !val.isEmpty() )
    {
			form->Left = val.asInt();
      if( form->Left >= ssze.X-50 )
        form->Left = ssze.X-50;
    }
		val =	stream->valueRead( esT("TOP"), EsVariant::null() );
		if( !val.isEmpty() )
    {
			form->Top = val.asInt();
      if( form->Top >= ssze.Y-50 )
        form->Top = ssze.Y-50;
    }
	}
}
//---------------------------------------------------------------------------

/// save form extent, state and position
///
void EsUtilities::formPosExtAndStateSave(const TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context)
{
	ES_ASSERT(form);
	ES_ASSERT(stream);

	EsStreamContextExistingOrNewScope scope(stream, context);
	if( scope.isOk() )
	{
		TWindowState ws = const_cast<TForm*>(form)->WindowState;
		stream->valueWrite( esT("STATE"), static_cast<int>(ws) );
		if( TWindowState::wsMinimized != ws )
		{
			stream->valueWrite( esT("LEFT"), const_cast<TForm*>(form)->Left );
			stream->valueWrite( esT("TOP"), const_cast<TForm*>(form)->Top );

      if( TWindowState::wsNormal == ws )
      {
			  stream->valueWrite( esT("WIDTH"), const_cast<TForm*>(form)->Width );
			  stream->valueWrite( esT("HEIGHT"), const_cast<TForm*>(form)->Height );
      }
		}
	}
}
//---------------------------------------------------------------------------

/// save form extent, state and position
///
void EsUtilities::formPosSave(const TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context)
{
	ES_ASSERT(form);
	ES_ASSERT(stream);

	EsStreamContextExistingOrNewScope scope(stream, context);
	if( scope.isOk() )
	{
		stream->valueWrite( esT("LEFT"), const_cast<TForm*>(form)->Left );
		stream->valueWrite( esT("TOP"), const_cast<TForm*>(form)->Top );
	}
}
//---------------------------------------------------------------------------

void EsUtilities::formPositionFix(TForm* frm)
{
	TForm* ref = nullptr;
	int rleft = 0;
	int rtop = 0;
	int rw = 0;
	int rh = 0;

	ES_ASSERT(frm);

	TFormPosition fp = frm->Position;

  if( TFormPosition::OwnerFormCenter == fp )
  {
    if( frm->Owner )
    {
      ref = dynamic_cast<TForm*>(frm->Owner);
      if( !ref )
        ref = dynamic_cast<TForm*>(Application->MainForm);
    }
    else
      fp = TFormPosition::MainFormCenter;
  }

  if( TFormPosition::MainFormCenter == fp )
  {
		ref = dynamic_cast<TForm*>(Application->MainForm);
    if(!ref)
      fp = TFormPosition::ScreenCenter;
  }

	if( TFormPosition::ScreenCenter == fp )
  {
    const TPoint& pnt = screenSizeGet().Round();
    rw = pnt.X;
    rh = pnt.Y;
  }

	if( ref )
	{
		rleft = ref->Left;
		rtop = ref->Top;
		rw = ref->Width;
		rh = ref->Height;
	}

	int w = frm->Width;
	int h = frm->Height;
	frm->Left = rleft + (rw-w)/2;
	frm->Top = rtop + (rh-h)/2;
}
//---------------------------------------------------------------------------

void EsUtilities::actionClientUnregisterFix(TComponent* obj)
{
  if( obj )
  {
    TApplication* a = Application;

    for(int idx = 0; idx < obj->ComponentCount; ++idx)
    {
      TFmxObject* o = dynamic_cast<TFmxObject*>(obj->Components[idx]);
      if( a && o )
      {
        a->UnregisterActionClient(o);
        actionClientUnregisterFix(o);
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsUtilities::errorShow(const EsString& error, bool warning /*= false*/, TCustomImageList* images /*= nullptr*/)
{
  errorShow(
    EsString::null(),
    error,
    warning,
    images
  );
}
//---------------------------------------------------------------------------

void EsUtilities::errorShow(const EsString& caption, const EsString& error, bool warning /*= false*/, TCustomImageList* images /*= nullptr*/)
{
  messageShow(
    caption,
    error,
    warning ?
      TMsgDlgType::mtWarning :
      TMsgDlgType::mtError,
    images
  );
}
//---------------------------------------------------------------------------

void EsUtilities::messageShow(const EsString& msg, TMsgDlgType type, TCustomImageList* images /*= nullptr*/, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter /*= nullptr*/)
{
  messageShow(
    EsString::null(),
    msg,
    type,
    TMsgDlgBtn::mbClose,
    images,
    customCaptionGetter
  );
}
//---------------------------------------------------------------------------

void EsUtilities::messageShow(const EsString& msg, TMsgDlgType type, TMsgDlgBtn btn /*= TMsgDlgBtn::mbClose*/, TCustomImageList* images /*= nullptr*/, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter /*= nullptr*/)
{
  messageShow(
    EsString::null(),
    msg,
    type,
    btn,
    images,
    customCaptionGetter
  );
}
//---------------------------------------------------------------------------

void EsUtilities::messageShow(const EsString& caption, const EsString& msg, TMsgDlgType type, TCustomImageList* images /*= nullptr*/, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter /*= nullptr*/)
{
  TDlgUserPrompt::execute(
    Application,
    TFormPosition::MainFormCenter,
    EmptyStr,
    caption.c_str(),
    msg.c_str(),
    type,
    TMsgDlgButtons() << TMsgDlgBtn::mbClose,
    EsVariant::null(),
    nullptr,
    images,
    -1,
    customCaptionGetter
  );
}
//---------------------------------------------------------------------------

void EsUtilities::messageShow(const EsString& caption, const EsString& msg, TMsgDlgType type, TMsgDlgBtn btn /*= TMsgDlgBtn::mbClose*/, TCustomImageList* images /*= nullptr*/, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter /*= nullptr*/)
{
  TDlgUserPrompt::execute(
    Application,
    TFormPosition::MainFormCenter,
    EmptyStr,
    caption.c_str(),
    msg.c_str(),
    type,
    TMsgDlgButtons() << btn,
    EsVariant::null(),
    nullptr,
    images,
    -1,
    customCaptionGetter
  );
}
//---------------------------------------------------------------------------

void EsUtilities::folderPick(TComponent* Owner, const EsString& title,
	const EsString& folder, EsDlgOnClose onClose,
  TCustomImageList* images /*= 0*/, const EsStringArray& roots /*= EsString::nullArray()*/)
{
  ES_ASSERT(onClose);

	TDlgBrowseForFolder* dlg = new TDlgBrowseForFolder(Owner);
	ES_ASSERT(dlg);

	dlg->Caption = title.c_str();
  dlg->showFilesSet(false);
  dlg->images = images;

  if( !roots.empty() )
	  dlg->rootFoldersSet( roots );
  else
  {
    dlg->rootFoldersSet(
#if ES_OS == ES_OS_WINDOWS
      EsPath::stdDriveNamesGet()
#else
      EsPath::stdDataRootsGet()
#endif
    );
  }

	dlg->currentFolderSet( folder );
	dlg->showModal( onClose );
}
//---------------------------------------------------------------------------

TMenuItem* EsUtilities::menuSeparatorAddTo(TMenuItem* parent, int atPos /*= -1*/)
{
	TMenuItem* item = new TMenuItem(parent);
  ES_ASSERT(item);
	item->Text = esT("-");
  item->StyleLookup = esT("menuseparatorstyle");

  if( -1 < atPos )
  	parent->InsertObject(atPos, item);
  else
    parent->AddObject(item);

  item->ApplyStyleLookup();

  return item;
}
//---------------------------------------------------------------------------

void EsUtilities::menuItemPropertiesCopyTo(TMenuItem* dest, TMenuItem* src)
{
  ES_ASSERT(dest);
  ES_ASSERT(src);
  ES_ASSERT(dest != src);

  dest->Text = src->Text;
//  dest->Name = src->Name;
  dest->StyleName = src->StyleName;
  dest->StyleLookup = src->StyleLookup;

  // Handle separators separately and explicitly
  if( "-" == dest->Text )
    dest->StyleLookup = esT("menuseparatorstyle");
  else if( dest->StyleLookup.IsEmpty() )
    dest->StyleLookup = esT("menuitemstyle");

  dest->Bitmap = src->Bitmap;
  dest->HelpContext = src->HelpContext;
  dest->HelpType = src->HelpType;
  dest->Height = src->Height;
  dest->AutoCheck = src->AutoCheck;
  dest->IsChecked = src->IsChecked;
  dest->Visible = src->Visible;
  dest->GroupIndex = src->GroupIndex;
  dest->RadioItem = src->RadioItem;
  dest->Tag = src->Tag;
  dest->TagFloat = src->TagFloat;
  dest->TagString = src->TagString;
  dest->ShortCut = src->ShortCut;
  // Event handlers
  dest->OnApplyStyleLookup = src->OnApplyStyleLookup;
  dest->OnClick = src->OnClick;
  dest->OnGesture = src->OnGesture;
  dest->OnTap = src->OnTap;
  // Copy action as a last operator, to allow it to override any previous
  // assignments, if neccessary
  dest->Action = src->Action;
}
//---------------------------------------------------------------------------

TMenuItem* EsUtilities::menuItemCloneAddTo(TMenuItem* parent, TMenuItem* src, int atPos /*= -1*/)
{
  TMenuItem* clone = new TMenuItem(parent);
  ES_ASSERT(clone);

  menuItemPropertiesCopyTo(
    clone,
    src
  );
  clone->ShowHint = true;

  if( src->ItemsCount )
    menuItemsCopyTo(
      clone,
      src,
      false,
      false
    );

  if( atPos > -1 )
    parent->InsertObject(
      atPos,
      clone
    );
  else
    parent->AddObject(clone);

  return clone;
}
//---------------------------------------------------------------------------

void EsUtilities::menuItemsCopyTo(
  TMenuItem* dest,
  TMenuItem* src,
  bool appendSeparator,
  bool atEnd,
  EsUtilities::EsMenuItemsContainer* ctr /*= nullptr*/)
{
  if(dest && src && src->ItemsCount)
  {
    if( appendSeparator && 0 == dest->ItemsCount )
      appendSeparator = false;

    bool prependSeparator = false;
    if( atEnd && 0 != dest->ItemsCount )
    {
      if( dest->Items[dest->ItemsCount-1]->Text != "-" )
        prependSeparator = true;
    }

    // insert context menu items on top of persistent items
    int iidx = atEnd ?
      dest->ItemsCount :
      0;

    if( prependSeparator )
    {
      TMenuItem* sep = menuSeparatorAddTo(
        dest,
        iidx++
      );

      if( ctr )
        ctr->push_back(sep);
    }

    // populate main item with src subitem's clones
    for(int sidx = 0; sidx < src->ItemsCount; ++sidx)
    {
      TMenuItem* sub = src->Items[sidx];
      TMenuItem* subClone = menuItemCloneAddTo(
        dest,
        sub,
        iidx++
      );

      if( ctr )
        ctr->push_back(subClone);
    }

    if( appendSeparator )
    {
      TMenuItem* sep = menuSeparatorAddTo(
        dest,
        iidx
      );

      if( ctr )
        ctr->push_back(sep);
    }

    dest->ApplyStyleLookup();
  }
}
//---------------------------------------------------------------------------

/// Calculate text rectangle. Use initialRect as calculation base. Use font, currently selected into canvas.
TRectF EsUtilities::textRectGet(TCanvas* canvas, const EsString& txt, bool wrap /*= false*/, const TRectF& initialRect /*= TRectF()*/)
{
	ES_ASSERT(canvas);
	TRectF r = initialRect;
	if( !txt.empty() && canvas )
	{
		TTextLayoutClass tlc = TTextLayoutManager::DefaultTextLayout;
		ES_ASSERT(tlc);
    EsArgsT args;

    args.push_back( TValue::_op_Implicit(canvas) );
		std::unique_ptr<TTextLayout> tl(
      dynamic_cast<TTextLayout*>(EsUtilities::NEW(tlc, args))
    );

    tl->Font = canvas->Font;
    tl->TopLeft.X = 0; tl->TopLeft.Y = 0;
    tl->Trimming = TTextTrimming::None;
    tl->WordWrap = wrap;
    if( wrap )
      tl->MaxSize.x = initialRect.Width();

    tl->Text = txt.c_str();
    r = tl->TextRect;
    r.TopLeft() = initialRect.TopLeft();

		r.Union( initialRect );
	}

	return r;
}
//---------------------------------------------------------------------------

/// Calculate text rectangle. Use initialRect as calculation base. Use specified font for text calculation.
TRectF EsUtilities::textRectGet(TFont* font, const EsString& txt, bool wrap /*= false*/, const TRectF& initialRect /*= TRectF()*/)
{
	ES_ASSERT(font);
	TCanvas* c = TCanvasManager::MeasureCanvas;
	ES_ASSERT(c);
	c->Font->Assign(font);
	return textRectGet(c, txt, wrap, initialRect);
}
//---------------------------------------------------------------------------

/// Calculate text rectangle. Use initialRect as calculation base. Use font settings of the specified control for calculation.
TRectF EsUtilities::textRectGet(TControl* ctrl, const EsString& txt, bool wrap /*= false*/, const TRectF& initialRect /*= TRectF()*/)
{
	ES_ASSERT(ctrl);
	TCanvas* c = ctrl->Canvas;
	if(!c)
		c = TCanvasManager::MeasureCanvas;
	ES_ASSERT(c);

  TValue ts = propertyGet(ctrl, esT("TextSettings"));
  if( !ts.IsEmpty )
  {
    TTextSettings* pts = dynamic_cast<TTextSettings*>(
      ts.AsObject()
    );

    if(pts)
  	  return textRectGet(pts->Font, txt, wrap, initialRect);
  }

 	return textRectGet(c, txt, wrap, initialRect);
}
//---------------------------------------------------------------------------

TRectF EsUtilities::listBoxRectGet(TCustomListBox* ctrl, float extraWidth /*= 24*/)
{
	ES_ASSERT(ctrl);
	TRectF result;

  if( extraWidth < 8 )
    extraWidth = 8;

	result.Right = ctrl->ClientWidth();
	float addWidth = ctrl->Padding->Left+ctrl->Padding->Right+extraWidth;

  TListBox* lbx = dynamic_cast<TListBox*>(ctrl);
	if( lbx && lbx->ShowCheckboxes ) //< Protected in TCustomListBox, so we got to try to cast to ListBox
		addWidth += 32;

	for(int idx = 0; idx < ctrl->Count; ++idx)
	{
		TListBoxItem* item = ctrl->ListItems[idx];
		ES_ASSERT(item);
		float width = textRectGet(ctrl, item->Text.c_str()).Width() + addWidth;
		if( result.Right < width )
			result.Right = width;
		result.Bottom += item->Height;
	}

	return result;
}
//---------------------------------------------------------------------------

TValue EsUtilities::toTValue(const EsVariant& var)
{
	ulong type = var.typeGet();
	switch( type )
	{
	case EsVariant::VAR_BOOL: 							// Variant has Boolean value
		return TValue::_op_Implicit( var.asBool() );
	case EsVariant::VAR_BYTE:   						// Variant has Byte value
		return TValue::_op_Implicit( var.asByte() );
	case EsVariant::VAR_CHAR:								// Variant has Character value
		return TValue::_op_Implicit( var.asChar() );
	case EsVariant::VAR_UINT:								// Variant has unsigned integer value. Its conversion rate is smaller than INT, not like in C
		return TValue::_op_Implicit( var.asULLong() );
	case EsVariant::VAR_INT:								// Variant has integer value
		return TValue::_op_Implicit( var.asInt() );
	case EsVariant::VAR_UINT64:							// Variant has unsigned 64-bit integer value. Its conversion rate is smaller than INT, not like in C
		return TValue::_op_Implicit( var.asULLong() );
	case EsVariant::VAR_INT64:							// Variant has 64-bit integer value
		return TValue::_op_Implicit( var.asLLong() );
	case EsVariant::VAR_DOUBLE:							// Variant has double value
		return TValue::_op_Implicit( var.asDouble() );
	case EsVariant::VAR_STRING:           	// Variant has string value
		return TValue::_op_Implicit( UnicodeString(var.asString().c_str()) );
	case EsVariant::VAR_STRING_COLLECTION:	// Variant has string collection value
	{
		std::unique_ptr<TStrings> list(new TStringList);
		toStringList(list.get(), var.asStringCollection());
		return TValue::_op_Implicit( list.get() );
	}
	case EsVariant::VAR_POINTER:						// Variant has generic pointer
    return TValue::_op_Implicit( reinterpret_cast<TObject*>(var.asPointer()) );
	case EsVariant::VAR_BIN_BUFFER:					// Variant has the binary buffer
	case EsVariant::VAR_OBJECT:							// Variant has an base interface of object
	case EsVariant::VAR_VARIANT_COLLECTION:	// Array of any type of elements, array of variants
		EsException::Throw(
      esT("Conversion from '%s' to TValue is not supported"),
			EsEnumLabelGet<EsVariantType>(type)
    );
		break;
	case EsVariant::VAR_EMPTY: 							// No value in the variant
		break;
	}

	return TValue::Empty;  // pacify compilers
}
//---------------------------------------------------------------------------

/// Convert EsDateTime to|from TDateTime
TDateTime EsUtilities::toTDateTime(const EsDateTime& dt)
{
	return TDateTime(
		dt.get_year(),
		dt.get_month(),
		dt.get_dayOfMonth(),
		dt.get_hours(),
		dt.get_minutes(),
		dt.get_seconds(),
		dt.get_milliseconds()
  );
}
//---------------------------------------------------------------------------

EsDateTime EsUtilities::fromTDateTime(const TDateTime& dt)
{
	unsigned short y, m, d, h, mm, s, ss;

	dt.DecodeDate(&y, &m, &d);
	dt.DecodeTime(&h, &mm, &s, &ss);
	EsDateTime result;
	result.compose(y, m, d, h, mm, s, ss);
	ES_ASSERT( result.isOk() );

	return result;
}
//---------------------------------------------------------------------------

TPointF EsUtilities::screenSizeGet()
{
  _di_IFMXScreenService iss;
  iss = TPlatformServices::Current->GetPlatformService(__uuidof(IFMXScreenService));
  if( iss )
    return iss->GetScreenSize();

  return TPointF(0, 0);
}
//---------------------------------------------------------------------------

static void propAssign(TRttiType* type, const UnicodeString& prop, void* dest, void* src)
{
  ES_ASSERT(type);
  TRttiProperty* p = type->GetProperty(prop);
  propAssignVal(p, dest, p->GetValue(src));
}
//---------------------------------------------------------------------------

void EsUtilities::styleApply(TObject* dest, TObject* container, const TStyledSettings& ss /*= AllStyledSettings*/)
{
  TFmxObject* sctl = dynamic_cast<TFmxObject*>(container);
  TFmxObject* dobj = dynamic_cast<TFmxObject*>(dest);
  const UnicodeString& sname = dobj->StyleName;

  if(dobj && sctl && !sname.IsEmpty())
  {
    TFmxObject* sres = sctl->FindStyleResource(sname);

    if( sres )
    {
      TRttiContext ctx;

      // Handle common shape styles
      TShape* sshape = dynamic_cast<TShape*>(sres);
      if(sshape)
      {
        TShape* dshape = dynamic_cast<TShape*>(dobj);
        if( dshape )
        {
          dshape->Stroke->Assign(sshape->Stroke);
          dshape->Fill->Assign(sshape->Fill);
          dshape->Opacity = sshape->Opacity;

          // Handle some common, as well as distinct shape family properties using rtti
          TRttiType* srtti = ctx.GetType( sshape->ClassType() );
          TRttiType* drtti = ctx.GetType( dshape->ClassType() );

          if( srtti == drtti )
          {
            if( srtti->Name == esT("TRectangle") ||
                srtti->Name == esT("TCalloutRectangle") )
            {
              propAssign(drtti, esT("Sides"), dshape, sshape);
              propAssign(drtti, esT("Corners"), dshape, sshape);
              propAssign(drtti, esT("CornerType"), dshape, sshape);
              propAssign(drtti, esT("XRadius"), dshape, sshape);
              propAssign(drtti, esT("YRadius"), dshape, sshape);
            }
            else if( srtti->Name == esT("TRoundRect") )
              propAssign(drtti, esT("Corners"), dshape, sshape);
          }
        }
      }
      else
      {
        // Handle text styles
        TText* stxt = dynamic_cast<TText*>(sres);
        if( stxt )
        {
          TText* dtxt = dynamic_cast<TText*>(dobj);
          if( dtxt )
          {
            if( ss.Contains(TStyledSetting::Family) )
              dtxt->TextSettings->Font->Family = stxt->TextSettings->Font->Family;

            if( ss.Contains(TStyledSetting::Size) )
              dtxt->TextSettings->Font->Size = stxt->TextSettings->Font->Size;

            if( ss.Contains(TStyledSetting::Style) )
              dtxt->TextSettings->Font->Style = stxt->TextSettings->Font->Style;

            if( ss.Contains(TStyledSetting::FontColor) )
            {
              dtxt->TextSettings->FontColor = stxt->TextSettings->FontColor;

              // Handle some extended text objects, like TMS HTML one
              TRttiType* drtti = ctx.GetType( dtxt->ClassType() );
              TRttiProperty* p = drtti->GetProperty(esT("Fill"));
              if( p )
              {
                TValue fill = p->GetValue(dtxt);
                TBrush* br = dynamic_cast<TBrush*>( fill.AsObject() );
                if( br )
                  br->Color = stxt->TextSettings->FontColor;
              }

              p = drtti->GetProperty(esT("Stroke"));
              if( p )
              {
                TValue stroke = p->GetValue(dtxt);
                TBrush* br = dynamic_cast<TBrush*>( stroke.AsObject() );
                if( br )
                  br->Color = stxt->TextSettings->FontColor;
              }

              if( dtxt->ClassName() == stxt->ClassName() )
              {
                p = drtti->GetProperty(esT("URLColor"));
                if( p )
                  p->SetValue( dtxt, p->GetValue(stxt) );
              }
            }

            if( ss.Contains(TStyledSetting::Other) )
            {
              dtxt->TextSettings->HorzAlign = stxt->TextSettings->HorzAlign;
              dtxt->TextSettings->VertAlign = stxt->TextSettings->VertAlign;
              dtxt->TextSettings->Trimming = stxt->TextSettings->Trimming;
              dtxt->TextSettings->WordWrap = stxt->TextSettings->WordWrap;
            }
          }
        }
        else
        {
          // Handle image styling
          TImage* simg = dynamic_cast<TImage*>(sres);
          if( simg )
          {
            TImage* dimg = dynamic_cast<TImage*>(dobj);
            if( dimg )
              dimg->MultiResBitmap = simg->MultiResBitmap;
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsUtilities::styleCustomFixup(
  const UnicodeString& styleNameCustom,
  TStyleBook* container,
  const UnicodeString& styleNameDef,
  const UnicodeString& styleElt
)
{
  ES_ASSERT(container);

  TStyleCollectionItem* item = container->CurrentItem;
  if( !item )
    return;

  TFmxObject* style = item->Style;

  if( !style )
    return;

  TFmxObject* dstyle = style->FindStyleResource(styleNameCustom);
  if( !dstyle )
    return;

  TFmxObject* sstyle = style->FindStyleResource(styleNameDef);
  if( !sstyle )
    style = TStyleManager::ActiveStyle(0);

  if( !style )
    return;

  sstyle = style->FindStyleResource(styleNameDef);
  if( !sstyle )
    return;

  TFmxObject* sstyleElt = sstyle->FindStyleResource(styleElt);
  TFmxObject* dstyleElt = dstyle->FindStyleResource(styleElt);

  if( !sstyleElt || !dstyleElt )
    return;

  int idx = dstyleElt->Index;
  dstyle->RemoveObject(dstyleElt);
  delete dstyleElt;
  dstyleElt = 0;

  dstyleElt = sstyleElt->Clone(dstyle);
  dstyle->InsertObject(idx, dstyleElt);
}
//---------------------------------------------------------------------------

static void internalActionClientsFind(TComponent* container, TBasicAction* subj,
  EsUtilities::EsActionClientsContainer& result)
{
  for( int idx = 0; idx < container->ComponentCount; ++idx )
  {
    TComponent* comp = container->Components[idx];
    TFmxObject* obj = dynamic_cast<TFmxObject*>(comp);

    TRttiContext ctx;
    if( obj )
    {
      TRttiType* otype = ctx.GetType( obj->ClassType() );
      ES_ASSERT(otype);

      TRttiProperty* prop = otype->GetProperty(esT("Action"));
      if( prop )
      {
        TValue aval = prop->GetValue(obj);
        TBasicAction* act = dynamic_cast<TBasicAction*>(aval.AsObject());

        if( aval.IsObject() && subj == act )
          result.push_back(obj);
      }
    }

    if( comp->ComponentCount )
      internalActionClientsFind(comp, subj, result);
  }
}
//---------------------------------------------------------------------------

EsUtilities::EsActionClientsContainer EsUtilities::actionClientsFind(TComponent* container, TBasicAction* subj)
{
  EsActionClientsContainer result;

  if( subj && container )
    internalActionClientsFind(container, subj, result);

  return result;
}
//---------------------------------------------------------------------------

int EsUtilities::imageIndexFindByName(TCustomImageList* src, const UnicodeString& name)
{
  if( src && !name.IsEmpty() )
  {
    for(int idx = 0; idx < src->Destination->Count; ++idx)
    {
      TCustomDestinationItem* destItem = src->Destination->Items[idx];

      if( 0 < destItem->LayersCount() )
      {
        TLayer* layer0 = destItem->Layers->Items[0];
        ES_ASSERT(layer0);

        if( name == layer0->Name )
          return idx;
      }
    }
  }

  return -1;
}
//---------------------------------------------------------------------------

int EsUtilities::imageSrcIndexFindByName(TCustomImageList* src, const UnicodeString& name)
{
  if( src && !name.IsEmpty() )
  {
    for(int idx = 0; idx < src->Source->Count; ++idx)
      if( name == src->Source->Items[idx]->Name )
        return idx;
  }

  return -1;
}
//---------------------------------------------------------------------------

TBitmap* EsUtilities::imageListItemGetByName(TCustomImageList* src, const TSizeF& sze, const UnicodeString& name)
{
  // NB! We make an assumption, that in our applications each source item is
  // corresponding to 1 destination item with the same index, as the source one
  //
  if( src && !name.IsEmpty() )
  {
    for(int idx = 0; idx < src->Source->Count; ++idx)
      if( name == src->Source->Items[idx]->Name )
        return src->Bitmap(sze, idx);
  }

  return 0;
}
//---------------------------------------------------------------------------

void EsUtilities::imageListItemCopyByName(TCustomImageList* dest, TCustomImageList* src, const UnicodeString& name)
{
  // NB! We make an assumption, that in our applications each source item is
  // corresponding to 1 destination item with the same index, as the source one
  //
  if( dest != src && dest && src )
  {
    int idx = imageSrcIndexFindByName(src, name);
    if( -1 < idx  )
    {
      TCustomSourceItem* srcSrc = src->Source->Items[idx];
      ES_ASSERT(srcSrc);

      idx = imageIndexFindByName(src, name);
      if( -1 < idx )
      {
        TCustomDestinationItem* srcDest = src->Destination->Items[idx];

        TCustomSourceItem* destSrc = dest->Source->Add();
        ES_ASSERT(destSrc);

        destSrc->Assign( srcSrc );

        TCustomDestinationItem* destDest = dest->Destination->Add();
        ES_ASSERT(destDest);
        destDest->Assign( srcDest );
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsUtilities::glyphSizeUpdateFromImage(TGlyph* glyph)
{
  // Update glyph size, using its contents
  if( glyph )
  {
    TCustomImageList* imgs = glyph->Images;
    int idx = glyph->ImageIndex;
    if( imgs && -1 < idx && idx < imgs->Destination->Count )
    {
      TCustomDestinationItem* item = imgs->Destination->Items[idx];
      ES_ASSERT(item);

      TLayer* lay = item->Layers->Items[0];
      ES_ASSERT(lay);

      const TRectF& r = lay->SourceRect->Rect;
      glyph->Size->Size = r.Size;
    }
  }
}
//---------------------------------------------------------------------------

void EsUtilities::styledContolHeightAdjust(TObject* subj, int minH, bool fallbackToPlatformDefault /*= true*/)
{
  TStyledControl* ctl = dynamic_cast<TStyledControl*>(subj);

  if( ctl )
  {
    EsUpdateLocker<TControl> lock( dynamic_cast<TControl*>(ctl->Parent) );

    const TSizeF& defSz = ctl->Size->DefaultValue;
    const TSizeF& sz = ctl->AdjustSizeValue;
    TAdjustType atype = ctl->AdjustType;

    TGridPanelLayout::TControlItem* item = nullptr;
    TGridPanelLayout::TCellItem* row = nullptr;
    float extraH = 0;

    TLayout* middle = dynamic_cast<TLayout*>(ctl->Parent); //< Extra check if we stuff this control into middle helper TLayout
    TGridPanelLayout* gpl = dynamic_cast<TGridPanelLayout*>(middle ? middle->Parent : ctl->Parent);
    if( gpl )
    {
      extraH =
        gpl->Padding->Top + gpl->Padding->Bottom +
        ctl->Margins->Top + ctl->Margins->Bottom;
      int idx = gpl->ControlCollection->IndexOf(
        middle ?
          reinterpret_cast<TControl*>(middle) :
          reinterpret_cast<TControl*>(ctl)
        );

      if(-1 < idx)
      {
        item = gpl->ControlCollection->Items[idx];
        row = gpl->RowCollection->Items[item->Row];
      }
    }

    int newH = minH;
    switch( atype )
    {
    case TAdjustType::FixedHeight:
    case TAdjustType::FixedSize:
      if( newH < sz.Height )
        newH = sz.Height;
      break;
    default:
      if( fallbackToPlatformDefault &&
          newH < defSz.Height
      )
        newH = defSz.Height;
      break;
    }


    ctl->Height = newH;
    if( row &&
        TGridPanelLayout::TSizeStyle::Absolute == row->SizeStyle
      )
    {
      if( row->Value < newH + extraH )
        row->Value = newH + extraH;

      float h = 0;
      for(int idx = 0; idx < gpl->RowCollection->Count; ++idx)
      {
        row = gpl->RowCollection->Items[idx];
        if( TGridPanelLayout::TSizeStyle::Absolute == row->SizeStyle )
          h += row->Value;
      }
      h += gpl->Padding->Top + gpl->Padding->Bottom;

      if( gpl->Height < h )
        gpl->Height = h;
    }
  }
}
//---------------------------------------------------------------------------

void EsUtilities::gridPanelLayoutForceApplyStyle(TGridPanelLayout* gpl)
{
  ES_ASSERT(gpl);

  for(int idx = 0; idx < gpl->ControlCollection->Count; ++idx)
  {
    TGridPanelLayout::TControlItem* item = gpl->ControlCollection->Items[idx];
    TStyledControl* sctl = dynamic_cast<TStyledControl*>(item->Control);
    if( sctl )
      sctl->ApplyStyleLookup();
  }
}
//---------------------------------------------------------------------------

void EsUtilities::buttonImgStyleTuneup(TObject* subj, bool doStretch, int reqW, int reqH,
  TAlignLayout align /*= TAlignLayout::Fit*/ )
{
  if( 0 == reqW )
    reqW = 32;
  if( 0 == reqH )
    reqH = 32;

  TSpeedButton* obj = dynamic_cast<TSpeedButton*>(subj);
  if( obj )
  {
#if 0x0720 <= __CODEGEARC__
    TLayout* lay = dynamic_cast<TLayout*>(obj->FindStyleResource("speedbuttonstyle"));
    if( lay )
    {
      lay->FixedSize = TSize(0, 0);
    }
#endif

    TText* txt = dynamic_cast<TText*>(obj->FindStyleResource("text"));
    if( txt )
      txt->Visible = false;

    TSizeF sze = obj->Size->Size;
    if( sze.cx < reqW+2 )
      sze.cx = reqW+2;
    if( sze.cy < reqH+2 )
      sze.cy = reqH+2;
    obj->Size->Size = sze;

    TGlyph* img = dynamic_cast<TGlyph*>(obj->FindStyleResource("glyphstyle"));
    if( img )
    {
      EsUpdateLocker<TGlyph> lock(img);
      img->Padding->Rect = img->Padding->DefaultValue;
      img->Margins->Rect = img->Margins->DefaultValue;
      img->Align = TAlignLayout::None;
      img->Stretch = doStretch;

#ifdef ES_MODERN_CPP
      TSize sze = {
        reqW,
        reqH
      };
#else
      TSize sze(
        reqW,
        reqH
      );
#endif

      img->Size->DefaultValue = sze;
      img->Size->Size = sze;
      img->Align = align;
    }
  }
}
//---------------------------------------------------------------------------
