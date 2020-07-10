//---------------------------------------------------------------------------
#ifndef EsCoreGuiUtilitiesH
#define EsCoreGuiUtilitiesH

/// @file EsCoreGuiUtilities.h
/// Generic graphics utility classes and services, implemented for FMX framework

#include <FMX.Layouts.hpp>
//---------------------------------------------------------------------------

/// Templated update locker utility class
///
/// For paintable objects
template <typename T>
class EsUpdateLocker
{
public:
  explicit EsUpdateLocker( T* obj ) :
  m_obj(obj)
  {
    if( m_obj )
      m_obj->BeginUpdate();
  }

  void release()
  {
    if( m_obj )
    {
      m_obj->EndUpdate();
      m_obj = nullptr;
    }
  }

  ~EsUpdateLocker()
  {
    release();
  }

private:
  T* m_obj;

  EsUpdateLocker() ES_REMOVEDECL;
  EsUpdateLocker( const EsUpdateLocker& ) ES_REMOVEDECL;
  EsUpdateLocker& operator=( const EsUpdateLocker& ) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Scoped disable and re-enable (mainly used in timer event handlers)
///
template <typename T>
class EsScopedDisabler
{
public:
  explicit EsScopedDisabler( T* obj ) :
  m_obj(obj)
  {
    if( m_obj )
      m_obj->Enabled = false;
  }

  void release()
  {
    if( m_obj )
    {
      m_obj->Enabled = true;
      m_obj = nullptr;
    }
  }

  ~EsScopedDisabler()
  {
    release();
  }

private:
  T* m_obj;
  EsScopedDisabler() ES_REMOVEDECL;
  EsScopedDisabler( const EsScopedDisabler& ) ES_REMOVEDECL;
  EsScopedDisabler& operator=( const EsScopedDisabler& ) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Locally scoped FMX object
///
template <typename T>
class EsScopedObject
{
private:
  typedef EsScopedObject<T> ThisTypeT;
  typedef void (ThisTypeT:: *safebool_type)() const;
  void this_type_does_not_support_comparisons() const {}

public:
  explicit EsScopedObject( T* obj ) :
  m_obj(obj)
  {
  }

  void release()
  {
    if( m_obj )
    {
      m_obj->Parent = nullptr;
      if( m_obj->Owner )
        m_obj->Owner->RemoveComponent(m_obj);
      m_obj->Release();
      m_obj = nullptr;
    }
  }

  operator ThisTypeT::safebool_type () const
  {
    return (nullptr != m_obj) ?
      &ThisTypeT::this_type_does_not_support_comparisons :
      nullptr;
  }

  T* get()
  {
    return m_obj;
  }

  const T* get() const
  {
    return m_obj;
  }

  T* operator -> ()
  {
    ES_ASSERT(m_obj);
    return m_obj;
  }

  const T* operator -> () const
  {
    ES_ASSERT(m_obj);
    return m_obj;
  }

  ~EsScopedObject()
  {
    release();
  }

private:
  T* m_obj;
  EsScopedObject() ES_REMOVEDECL;
  EsScopedObject( const EsScopedObject& ) ES_REMOVEDECL;
  EsScopedObject& operator=( const EsScopedObject& ) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

template <typename T>
bool operator!=(const EsScopedObject<T>& lhs, const EsScopedObject<T>& rhs)
{
  lhs.this_type_does_not_support_comparisons();
  return false;
}
//---------------------------------------------------------------------------

template <typename T>
bool operator==(const EsScopedObject<T>& lhs, const EsScopedObject<T>& rhs)
{
  lhs.this_type_does_not_support_comparisons();
  return false;
}
//---------------------------------------------------------------------------

/// FMX comparison interface invocation template
/// Idea taken from here: http://mojelder.hatenablog.com/entry/2015/07/24/100846
///
enum EsDummyT {};
template <
  typename IntfT,
  typename ImplT,
  typename RetT,
  typename Param1T = EsDummyT,
  typename Param2T = EsDummyT
>
class EsFmxIntfInvoker : public TCppInterfacedObject< IntfT >
{
public:
  EsFmxIntfInvoker(ImplT impl) : m_impl(impl) {}

  virtual RetT __fastcall Invoke(Param1T* _1, Param2T* _2)
  {
    return m_impl(_1, _2);
  }

private:
  ImplT m_impl;
};
//---------------------------------------------------------------------------

/// Helper class for action list categorization
///
class ESCORE_GUI_CLASS EsActionListCategorizer
{
private:
  typedef std::multimap<EsString::HashT, TContainedAction*> EsCategorizedActionsT;

public:
  typedef std::vector<TContainedAction*> EsActionsT;

public:
  EsActionListCategorizer( TContainedActionList* al = nullptr );
  void rebuild(TContainedActionList* al);

  bool empty() const { return m_categories.empty(); }
  inline const EsString::Array& categoriesGet() const { return m_categories; }
  /// Get actions by category name
  EsActionsT actionsGet(const EsString& category) const;
  /// Get all actions (ungrouped, but category-ordered access)
  EsActionsT actionsGet() const;
  /// Get action by its object name. If action is not found - return 0
  TContainedAction* actionGet(const EsString& name) const;

protected:
  EsString::Array m_categories;
  EsCategorizedActionsT m_acts;

private:
  EsActionListCategorizer(const EsActionListCategorizer&) ES_REMOVEDECL;
  EsActionListCategorizer& operator=(const EsActionListCategorizer&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

class ESCORE_GUI_CLASS EsSingleInstanceGuard
{
public:
  EsSingleInstanceGuard(const UnicodeString& name);
  ~EsSingleInstanceGuard();

  void notifyInstance();

  __property bool ok = {read=m_ok};
  __property bool duplicate = {read=m_duplicate};
#if ES_OS == ES_OS_WINDOWS
  __property UINT messageId = {read=m_msgId};
#endif

protected:
  UnicodeString m_name;
#if ES_OS == ES_OS_WINDOWS
  HANDLE m_mx;
  UINT m_msgId;
#endif
  bool m_ok;
  bool m_duplicate;

private:
  EsSingleInstanceGuard() ES_REMOVEDECL;
  EsSingleInstanceGuard(const EsSingleInstanceGuard&) ES_REMOVEDECL;
  EsSingleInstanceGuard& operator=(const EsSingleInstanceGuard&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
class ESCORE_GUI_CLASS EsSingleInstanceGuardMsgHook
{
public:
  EsSingleInstanceGuardMsgHook(UINT guardMsgId);
  ~EsSingleInstanceGuardMsgHook();

  __property TNotifyEvent onMultiInstanceNotify = {read=m_onNotify, write=m_onNotify};

protected:
  static LRESULT CALLBACK HookWndProc(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
  );

protected:
  TNotifyEvent m_onNotify;
  HWND m_hwnd;
  LONG_PTR m_oldProc;
  UINT m_guardMsgId;
  bool m_installed;
  static EsSingleInstanceGuardMsgHook* s_inst;

private:
  EsSingleInstanceGuardMsgHook() ES_REMOVEDECL;
  EsSingleInstanceGuardMsgHook(const EsSingleInstanceGuardMsgHook&) ES_REMOVEDECL;
  EsSingleInstanceGuardMsgHook& operator=(const EsSingleInstanceGuardMsgHook&) ES_REMOVEDECL;
};
#endif  // ES_OS == ES_OS_WINDOWS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Utilities namespace
///
namespace EsUtilities
{

template <typename ComponentT>
static ComponentT* componentFindExisting(TComponent* owner, UnicodeString cmpName = EmptyStr)
{
  if( owner )
  {
    if( cmpName.IsEmpty() )
    {
      TRttiContext ctx;
      TRttiType* type = ctx.GetType( __classid( ComponentT ) );
      ES_ASSERT(type);
      cmpName = type->Name;
    }

    ES_ASSERT(!cmpName.IsEmpty());
    ComponentT* cmp = dynamic_cast<ComponentT*>(owner->FindComponent(cmpName));

    return cmp;
  }

  return nullptr;
}

template <typename ComponentT>
static ComponentT* componentCreateOrFindExisting(TComponent* owner, UnicodeString compName = EmptyStr)
{
  if( owner )
  {
    if( compName.IsEmpty() )
    {
      TRttiContext ctx;
      TRttiType* type = ctx.GetType( __classid( ComponentT ) );
      ES_ASSERT(type);
      compName = type->Name;
    }

    ES_ASSERT(!compName.IsEmpty());
    ComponentT* cmp = dynamic_cast<ComponentT*>(owner->FindComponent(compName));
    if( !cmp )
    {
      cmp = new ComponentT(owner);
      cmp->Name = compName;
    }

    return cmp;
  }

  return nullptr;
}

template <typename ComponentT>
static ComponentT* childFind(TFmxObject* parent, UnicodeString name)
{
  if( parent )
  {
    ES_ASSERT(!name.IsEmpty());
    for(int idx = 0; idx < parent->ChildrenCount; ++idx )
    {
      ComponentT* cmp = dynamic_cast<ComponentT*>(
        parent->Children->Items[idx]
      );
      if( cmp && name == cmp->Name )
        return cmp;
    }
  }

  return nullptr;
}

/// Create Delphi object instance from its metaclass
typedef std::vector<TValue> EsArgsT;
ESCORE_GUI_FUNC( TObject*, NEW(TMetaClass* mc, const EsArgsT& args) );

/// Set Delphi object property by its name
ESCORE_GUI_FUNC( void, propertySet(TObject* obj, const EsString& prop, const TValue& val) );

/// Get Delphi object property by its name
ESCORE_GUI_FUNC( TValue, propertyGet(TObject* obj, const EsString& prop) );

/// Iterate child objects and perform some action on each of them.
/// To drop iteration, return false from iterator, otherwise, return true to continue
///
typedef bool (__closure *EsFmxChildrenIteratorFn)(TFmxObject& child);
ESCORE_GUI_FUNC( void, childrenIterate(TFmxObject* obj, EsFmxChildrenIteratorFn fn) );

/// Convert our string array to string list contents
///
ESCORE_GUI_FUNC( void, toStringList(TStrings* dest, const EsString::Array& src ) );

/// Convert our string array to string list contents, using i18n
///
ESCORE_GUI_FUNC( void, toStringListI18n(TStrings* dest, const EsString::Array& src ) );

/// Convert string list contents to our string array
///
ESCORE_GUI_FUNC( void, fromStringList(EsString::Array& dest, const TStrings* src) );

/// Find child object by its name. Return 0 if not found
///
ESCORE_GUI_FUNC( TFmxObject*, childFindByName(TFmxObject* src, const UnicodeString& name) );

/// Forcibly update all actions in action list
///
ESCORE_GUI_FUNC( void, actionListUpdate(TContainedActionList* al) );

/// Find action in action list by its component name
///
ESCORE_GUI_FUNC( TContainedAction*, actionListFind(TContainedActionList* al, const UnicodeString& actionName) );

/// Return extent of first-level contents, i.e. all immediate children
/// of ctl container
///
ESCORE_GUI_FUNC( TPointF, contentsExtentGet(TControl* ctl, int start = 0) );

/// Remove all components owned by the cmp
///
ESCORE_GUI_FUNC( void, componentsRemoveFrom(TComponent* cmp) );

/// Load form extent, state and position
///
ESCORE_GUI_FUNC( void, formPosExtAndStateLoad(TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context) );

/// Load form position
///
ESCORE_GUI_FUNC( void, formPosLoad(TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context) );

/// Save form extent, state and position
///
ESCORE_GUI_FUNC( void, formPosExtAndStateSave(const TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context) );

/// Save form position
///
ESCORE_GUI_FUNC( void, formPosSave(const TForm* form, const EsStreamIntf::Ptr& stream, const EsString& context) );

/// Firemonkey bug fix for TForm not handling specified position property
///
ESCORE_GUI_FUNC( void, formPositionFix(TForm* frm) );

/// Firemonkey bug fix for not unregistering contained fmxObjects from
/// Application-wide action client list
///
ESCORE_GUI_FUNC( void, actionClientUnregisterFix(TComponent* obj) );

/// Firemonkey error message box
///
ESCORE_GUI_FUNC( void, errorShow(const EsString& caption, const EsString& error, bool warning = false, TCustomImageList* images = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );
ESCORE_GUI_FUNC( void, errorShow(const EsString& error, bool warning = false, TCustomImageList* images = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );

/// User message box
ESCORE_GUI_FUNC( void, messageShow(const EsString& caption, const EsString& msg, TMsgDlgType type, TMsgDlgBtn btn = TMsgDlgBtn::mbClose, TCustomImageList* images = nullptr, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );
ESCORE_GUI_FUNC( void, messageShow(const EsString& caption, const EsString& msg, TMsgDlgType type, TCustomImageList* images = nullptr, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );
ESCORE_GUI_FUNC( void, messageShow(const EsString& msg, TMsgDlgType type, TMsgDlgBtn btn = TMsgDlgBtn::mbClose, TCustomImageList* images = nullptr, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );
ESCORE_GUI_FUNC( void, messageShow(const EsString& msg, TMsgDlgType type, TCustomImageList* images = nullptr, TDlgUserPrompt::CustomButtonCaptionGetterT customCaptionGetter = nullptr, TFormPosition pos = TFormPosition::MainFormCenter) );

/// Show folder picker dialog
///
ESCORE_GUI_FUNC( void, folderPick(TComponent* Owner, const EsString& title,
  const EsString& folder, EsDlgOnClose onClose, TCustomImageList* images = nullptr,
  const EsStringArray& roots = EsString::nullArray()
) );

/// Copy all menu item properties from src to dest
///
ESCORE_GUI_FUNC( void, menuItemPropertiesCopyTo(TMenuItem* dest, TMenuItem* src) );

/// Append menu separator item to the parent menu, and return pointer to it
/// If atPos is > -1, separator is inserted at specified position
///
ESCORE_GUI_FUNC( TMenuItem*, menuSeparatorAddTo(TMenuItem* parent, int atPos = -1) );

/// Append clone of menu item (with all its sub-items) to the parent menu, and return pointer to it
/// If atPos is > -1, item is inserted at specified position
///
ESCORE_GUI_FUNC( TMenuItem*, menuItemCloneAddTo(TMenuItem* parent, TMenuItem* src, int atPos = -1) );

/// Copy menu items from src to dest. If appendSeparator is true, separator item is appended after all the copied items,
/// but only if there are other items in dest menu. Optional container, if not null,
/// will aggregate copied items. Only top-level copied items got aggregated in the container.
///
typedef std::list<TMenuItem*> EsMenuItemsContainer;
ESCORE_GUI_FUNC( void, menuItemsCopyTo(TMenuItem* dest, TMenuItem* src, bool appendSeparator, bool atEnd, EsMenuItemsContainer* ctr = nullptr) );

/// Calculate text rectangle
///
/// Calculate text rectangle. Use initialRect as calculation base. Use font, currently selected into canvas.
ESCORE_GUI_FUNC( TRectF, textRectGet(TCanvas* canvas, const EsString& txt, bool wrap = false, const TRectF& initialRect = TRectF()) );

/// Calculate text rectangle. Use initialRect as calculation base. Use specified font for text calculation.
ESCORE_GUI_FUNC( TRectF, textRectGet(TFont* font, const EsString& txt, bool wrap = false, const TRectF& initialRect = TRectF()) );

/// Calculate text rectangle. Use initialRect as calculation base. Use font settings of the specified control for calculation.
ESCORE_GUI_FUNC( TRectF, textRectGet(TControl* ctrl, const EsString& txt, bool wrap = false, const TRectF& initialRect = TRectF()) );

/// Calculate list items rectangle using specified control as font provider for calculation
ESCORE_GUI_FUNC( TRectF, listBoxRectGet(TCustomListBox* ctrl, float extraWidth = 24) );

/// EsVariant to TValue conversion
ESCORE_GUI_FUNC( TValue, toTValue(const EsVariant& var) );

/// Convert EsDateTime to|from TDateTime
ESCORE_GUI_FUNC( TDateTime, toTDateTime(const EsDateTime& dt) );
ESCORE_GUI_FUNC( EsDateTime, fromTDateTime(const TDateTime& dt) );

/// Retrieve screen rectangle
ESCORE_GUI_FUNC( TPointF, screenSizeGet() );

/// Apply style to style-less primitive object (usually some shape), using styled container, if it's
/// derived from TStyledControl. In addition, if text primitive is styled, ss parameter
/// governs text settings to be used from the style resource object
///
ESCORE_GUI_FUNC( void, styleApply(TObject* dest, TObject* container, const TStyledSettings& ss = AllStyledSettings) );

/// @brief Apply fixup to custom style from the default style.
///
/// @param styleNameCustom  custom style name which will be fixed using the default one
/// @param container        styles container which holds all the styles
/// @param styleNameDef     default style name which is used as a fixup source
/// @param styleElt         style sub element which will be copied from the default one
///
ESCORE_GUI_FUNC( void, styleCustomFixup(const UnicodeString& styleNameCustom, TStyleBook* container, const UnicodeString& styleNameDef, const UnicodeString& styleElt) );

/// Look up an object, which have specific action set.
/// If no object is found, null is returned
///
typedef std::list<TFmxObject*> EsActionClientsContainer;
ESCORE_GUI_FUNC( EsActionClientsContainer, actionClientsFind(TComponent* container, TBasicAction* subj) );

/// Lookup image list, and return its 'destination' AKA 'actual image' item index by name, if one exists. Otherwise, return -1
ESCORE_GUI_FUNC( int, imageIndexFindByName(TCustomImageList* src, const UnicodeString& name) );

/// Lookup image list, and return its 'source' item index by name, if one exists. Otherwise, return -1
ESCORE_GUI_FUNC( int, imageSrcIndexFindByName(TCustomImageList* src, const UnicodeString& name) );

/// Copy item from src image list to the dest one, by its name
ESCORE_GUI_FUNC( void, imageListItemCopyByName(TCustomImageList* dest, TCustomImageList* src, const UnicodeString& name) );

/// Return image from image list by name
ESCORE_GUI_FUNC( TBitmap*, imageListItemGetByName(TCustomImageList* src, const TSizeF& sze, const UnicodeString& name) );

/// Update TGlyph size using its assigned image item
ESCORE_GUI_FUNC( void, glyphSizeUpdateFromImage(TGlyph* glyph) );

/// Update control height using fixed height in style
ESCORE_GUI_FUNC( void, styledContolHeightAdjust(TObject* subj, int minH, bool fallbackToPlatformDefault = true) );

/// Force style apply to each styled control in TGridPanelLayout
ESCORE_GUI_FUNC( void, gridPanelLayoutForceApplyStyle(TGridPanelLayout* gpl) );

ESCORE_GUI_FUNC( void, buttonImgStyleTuneup(TObject* subj, bool doStretch = true,
  int reqW = 0, int reqH = 0, TAlignLayout align = TAlignLayout::Fit) );

}

#define _S(x)  _(x).c_str()

//---------------------------------------------------------------------------
#endif
