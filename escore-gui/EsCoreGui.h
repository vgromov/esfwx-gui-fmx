#ifndef EsCoreGuiH
#define EsCoreGuiH
//---------------------------------------------------------------------------

#include <FMX.Objects.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Edit.hpp>
#include <FMX.NumberBox.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.DateTimeCtrls.hpp>

/// Object configuration pane interface
///
ES_DECL_INTF_BEGIN( 8CC19CF9, D0B24b4c, 8D361982, 26E276BA, EsReflectedObjectConfigPaneIntf)
	ES_DECL_INTF_METHOD(EsString, getObjectName)() const = 0;
	/// Access pane owner form
	ES_DECL_INTF_METHOD(TForm*, getOwner)() = 0;
	/// Update pane controls from linked Reflected object source
	ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& obj ) = 0;
	/// Update Reflected object properties from linked controls
	ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& obj ) const = 0;
ES_DECL_INTF_END

/// Reflectd property link classes
///
class DELPHICLASS EsReflectedClassDataSource;
class DELPHICLASS EsReflectedClassPropertyLink;

/// Reflected property GUI link base class
///
typedef void __fastcall (__closure *EsOnSrcObjectSet)(EsReflectedClassPropertyLink* sender);
typedef void __fastcall (__closure *EsOnCtlStateChanged)(EsReflectedClassPropertyLink* sender);
typedef void __fastcall (__closure *EsOnCtlValInvalid)(EsReflectedClassPropertyLink* sender, const EsVariant& val, EsVariant& suggestedVal);

typedef std::multimap< ulong, EsReflectedClassPropertyLink* > EsReflectedClassPropertyLinksT;

class ESCORE_GUI_PKG EsReflectedClassPropertyLink : public TComponent
{
protected:
	__fastcall EsReflectedClassPropertyLink(TComponent* Owner, const EsString& propName);

public:
	virtual __fastcall ~EsReflectedClassPropertyLink();

  void i18nInit();

	bool sourceIsEmpty() const;
	virtual bool isUnidirectional() const = 0;
	void sourceSet(EsReflectedClassDataSource* src, ulong priority = 1000);
	void updateControl(const EsReflectedClassIntf::Ptr& obj);
	void updateProperty(const EsReflectedClassIntf::Ptr& obj);
	void resetControlToDefault();

	const EsString& propertyNameGet() const;
  EsVariant attributeGet(const EsString& attr) const;
	EsVariant restrictionGet() const;
	EsVariant lookupGet() const;
	EsVariant defaultGet() const;

	__property UnicodeString label = { read = labelGet };
	__property UnicodeString description = { read = descriptionGet };
	__property bool visible = { read = visibleGet, write = visibleSet };
	__property bool enabled = { read = enabledGet, write = enabledSet };
  __property bool canWrite = { read = m_canWrite };

	__property EsOnSrcObjectSet onSrcObjectSet = {read = m_onSrcSet, write = m_onSrcSet};
  __property EsOnCtlStateChanged onCtlStateChanged = {read=m_onCtlChanged,write=m_onCtlChanged};
  __property EsOnCtlValInvalid onCtlValueInvalid = {read=m_onCtlValueInvalid,write=m_onCtlValueInvalid};

protected:
	UnicodeString __fastcall labelGet() const;
	UnicodeString __fastcall descriptionGet() const;
	bool __fastcall visibleGet() const;
	void __fastcall visibleSet(bool visible);
	bool __fastcall enabledGet() const;
	void __fastcall enabledSet(bool enabled);

	virtual void initialize() = 0;
	virtual void doControlValSet(const EsVariant& val) =  0;
	virtual EsVariant doControlValGet() const = 0;
  virtual bool doValidateControlVal(const EsVariant& val, EsVariant& suggestedVal) const; //< Optional in-place validator. Default implementation does nothing
  void doControlValueRollback(const EsVariant& val);

  virtual void __fastcall onI18nStringsUpdate(TObject* sender);

protected:
  EsI18nManagerScope m_i18nScope;
	EsOnSrcObjectSet m_onSrcSet;
  EsOnCtlStateChanged m_onCtlChanged;
  EsOnCtlValInvalid m_onCtlValueInvalid;
	EsReflectedClassDataSource* m_src;
	EsString m_propName;
  bool m_canWrite;

	ES_NO_DEFAULT_CTR(EsReflectedClassPropertyLink);
	ES_NON_COPYABLE(EsReflectedClassPropertyLink);
};

/// Property links collection for group access to reflected class properties
///
class ESCORE_GUI_PKG EsReflectedClassDataSource : public TComponent
{
public:
	__fastcall EsReflectedClassDataSource( TComponent* Owner, const EsString& className,
								const EsMetaclassIntf::Ptr& meta = EsMetaclassIntf::Ptr() );
	virtual __fastcall ~EsReflectedClassDataSource();

	const EsString& classNameGet() const;

  EsMetaclassIntf::Ptr metaGet() const;
	void metaSet(const EsMetaclassIntf::Ptr& meta);

	void link(EsReflectedClassPropertyLink* link, ulong priority = 1000);
  EsReflectedClassPropertyLink* linkFind(const EsString& propName) const;

	void updateControls(const EsReflectedClassIntf::Ptr& obj);
	void updateObject(const EsReflectedClassIntf::Ptr& obj) const;
	void resetControlsToDefault();

  void i18nInit();

	__property bool readOnly = {read=m_readOnly, write=m_readOnly};

  __property TNotifyEvent onCtrlsUpdated = { read = m_onCtrlsUpdated, write = m_onCtrlsUpdated };

protected:
	void linkAdd(EsReflectedClassPropertyLink* link, ulong priority);
	void linkRemove(EsReflectedClassPropertyLink* link);
	void classNameCheck(const EsReflectedClassIntf::Ptr& obj) const;

protected:
	EsReflectedClassPropertyLinksT m_links;
	EsMetaclassIntf::Ptr m_meta;
	EsString m_className;
  TNotifyEvent m_onCtrlsUpdated;
	bool m_readOnly;

  ES_NON_COPYABLE(EsReflectedClassDataSource);

	friend class EsReflectedClassPropertyLink;
};

/// Property links collection for group access to reflected class properties.
/// Reflected class instance is held and owned by the source itself.
///
class DELPHICLASS EsReflectedClassContainerDataSource;
class ESCORE_GUI_PKG EsReflectedClassContainerDataSource : public EsReflectedClassDataSource
{
public:
	__fastcall EsReflectedClassContainerDataSource( TComponent* Owner, const EsReflectedClassIntf::Ptr& obj );
	__fastcall EsReflectedClassContainerDataSource(	TComponent* Owner, const EsString& className, const EsMetaclassIntf::Ptr& meta );
	virtual __fastcall ~EsReflectedClassContainerDataSource();

	/// Contained object access
  ///
  
  /// Assign object to container, update target property links, as appropriate.
  /// Only objects of type specified at construction time are allowed.
  /// If object of incompatible type is set, an exception is thrown.
  ///
	void objectSet(const EsReflectedClassIntf::Ptr& obj);
  
  /// Get currently set data object.
  /// If object is not null, data in object request update before an object is returned to caller
  ///
	EsReflectedClassIntf::Ptr objectGet() const;

  /// Properties
  ///
  
  /// Return true if data object is empty
  ///
	__property bool empty = {read=emptyGet};

  /// Return true if data object is valid
  ///
	__property bool valid = {read=validGet};

protected:
  /// Return true if data object is empty
  ///
	bool __fastcall emptyGet() const;
  
  /// Return true if data object is valid
  ///
	bool __fastcall validGet() const;

protected:
  /// Reflected data object, held and managed by container
  ///
	EsReflectedClassIntf::Ptr m_obj;

  ES_NON_COPYABLE(EsReflectedClassContainerDataSource);
};

/// Closure property link. The __closure is used to handle data updates to\from
/// linked reflected property.
///
typedef void __fastcall (__closure *EsControlValSet)(const EsVariant& val);
typedef EsVariant __fastcall (__closure *EsControlValGet)() const;

class DELPHICLASS EsClosurePropertyLink;
class ESCORE_GUI_PKG EsClosurePropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsClosurePropertyLink(const EsString& prop, TComponent* owner,
		EsControlValSet setter, EsControlValGet getter = nullptr);
  virtual __fastcall ~EsClosurePropertyLink();

	virtual bool isUnidirectional() const;

protected:
	virtual void initialize();
	virtual void doControlValSet(const EsVariant& val);
	virtual EsVariant doControlValGet() const;

protected:
	EsControlValSet m_setter;
	EsControlValGet m_getter;
};

/// RTTI property link
///
class DELPHICLASS EsRttiPropertyLink;
class ESCORE_GUI_PKG EsRttiPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsRttiPropertyLink(const EsString& prop, TComponent* cmp, const EsString& rttiProp);
  virtual __fastcall ~EsRttiPropertyLink();

 	virtual bool isUnidirectional() const;

  __property TComponent* component = {read=m_cmp};

protected:
	virtual void initialize();
	virtual void doControlValSet(const EsVariant& val);
	virtual EsVariant doControlValGet() const;

protected:
	TComponent* m_cmp;
	TRttiProperty* m_ptype;
};

/// RTTI property link for indexed reflected properties
///
class DELPHICLASS EsRttiPropertyLinkIndexed;
class DELPHICLASS EsRttiPropertyIndexLink;
class ESCORE_GUI_PKG EsRttiPropertyIndexLink : public TComponent
{
public:
  __fastcall EsRttiPropertyIndexLink(EsRttiPropertyLinkIndexed* container, ulong index, TComponent* cmp, const EsString& rttiProp);
  virtual __fastcall ~EsRttiPropertyIndexLink();

  void containerSet(EsRttiPropertyLinkIndexed* container);
  void doControlValSet(const EsVariant& val);
  ulong indexGet() const;

protected:
  EsRttiPropertyLinkIndexed* m_container;
  TComponent* m_cmp;
  TRttiProperty* m_ptype;
  ulong m_index;
};

class ESCORE_GUI_PKG EsRttiPropertyLinkIndexed : public EsReflectedClassPropertyLink
{
private:
	typedef std::map<ulong, EsRttiPropertyIndexLink*> IndexedLinksT;
	friend class EsRttiPropertyIndexLink;

public:
	__fastcall EsRttiPropertyLinkIndexed(const EsString& prop, TComponent* owner);
	virtual __fastcall ~EsRttiPropertyLinkIndexed();

	virtual bool isUnidirectional() const;
	void indexLinkAdd(ulong index, TComponent* cmp, const EsString& rttiProp);

protected:
	void linkAdd(EsRttiPropertyIndexLink* link);
	void linkRemove(EsRttiPropertyIndexLink* link);

protected:
	virtual void initialize();
	virtual void doControlValSet(const EsVariant& val);
	virtual EsVariant doControlValGet() const;

protected:
	IndexedLinksT m_idxLinks;
};

/// Combo box link
///
class DELPHICLASS EsComboBoxPropertyLink;
class ESCORE_GUI_PKG EsComboBoxPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsComboBoxPropertyLink(const EsString& prop, TComboBox* cbx, TLabel* lbl = nullptr, bool useLookup = false);
  virtual __fastcall ~EsComboBoxPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TComboBox* linkedCtl = {read=m_cbx};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
	TComboBox* m_cbx;
	TLabel* m_lbl;
  bool m_useLookup;
};

/// spin control porperty link
///
class DELPHICLASS EsSpinCtlPropertyLink;
class ESCORE_GUI_PKG EsSpinCtlPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsSpinCtlPropertyLink(const EsString& propName, TCustomEditBox* ctl, TLabel* lbl= nullptr);
  virtual __fastcall ~EsSpinCtlPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TCustomEditBox* linkedCtl = {read=m_ctl};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
	TCustomEditBox* m_ctl;
	TLabel* m_lbl;
};

/// spin control double link
///
class DELPHICLASS EsSpinCtlDoublePropertyLink;
class ESCORE_GUI_PKG EsSpinCtlDoublePropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsSpinCtlDoublePropertyLink(const EsString& propName, TCustomEditBox* ctl, TLabel* lbl = nullptr);
  virtual __fastcall ~EsSpinCtlDoublePropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TCustomEditBox* linkedCtl = {read=m_ctl};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;
	
protected:
	TCustomEditBox* m_ctl;
	TLabel* m_lbl;
};

/// text control link
///
class DELPHICLASS EsTextCtlPropertyLink;
class ESCORE_GUI_PKG EsTextCtlPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsTextCtlPropertyLink(const EsString& propName, TCustomEdit* ctl, TLabel* lbl = nullptr);
  virtual __fastcall ~EsTextCtlPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TCustomEdit* linkedCtl = {read=m_ctl};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
	TCustomEdit* m_ctl;
	TLabel* m_lbl;
};

/// checkbox property link
///
class DELPHICLASS EsCheckBoxPropertyLink;
class ESCORE_GUI_PKG EsCheckBoxPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsCheckBoxPropertyLink(const EsString& propName,	TCheckBox* cbx);
  virtual __fastcall ~EsCheckBoxPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TCustomEditBox* linkedCtl = {read=m_ctl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
	TCheckBox* m_ctl;
};

/// labelled switch property link
///
class DELPHICLASS EsSwitchPropertyLink;
class ESCORE_GUI_PKG EsSwitchPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsSwitchPropertyLink(const EsString& propName, TSwitch* cbx, TLabel* lbl = nullptr);
  virtual __fastcall ~EsSwitchPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TSwitch* linkedCtl = {read=m_ctl};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
	TSwitch* m_ctl;
	TLabel* m_lbl;
};

/// labelled time editor property link
///
class DELPHICLASS EsTimeEditPropertyLink;
class ESCORE_GUI_PKG EsTimeEditPropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsTimeEditPropertyLink(const EsString& propName, TCustomTimeEdit* ctl, TLabel* lbl = nullptr);
  virtual __fastcall ~EsTimeEditPropertyLink();

	virtual bool isUnidirectional() const ES_OVERRIDE;

  __property TCustomTimeEdit* linkedCtl = {read=m_ctl};
  __property TLabel* linkedLbl = {read=m_lbl};

protected:
	virtual void initialize() ES_OVERRIDE;

  virtual bool valueRestrict(EsDateTime& dt) const;

  virtual bool doValidateControlVal(const EsVariant& val, EsVariant& suggested) const ES_OVERRIDE;
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE;
	virtual EsVariant doControlValGet() const ES_OVERRIDE;

  virtual void __fastcall onI18nStringsUpdate(TObject* sender) ES_OVERRIDE;

protected:
  EsDateTime m_min;
  EsDateTime m_max;
	TCustomTimeEdit* m_ctl;
	TLabel* m_lbl;
  bool m_minAssigned;
  bool m_maxAssigned;
};

//---------------------------------------------------------------------------
#endif
