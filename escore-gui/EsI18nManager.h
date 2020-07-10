#ifndef EsI18nManagerH
#define EsI18nManagerH
//---------------------------------------------------------------------------

/// @file EsI18nManager.h
/// Ekosf core GUI translation manager component
///

/// Forward declarations
class EsI18nManagerScope;
class DELPHICLASS EsI18nManager;

/// Plain pointer types (dealing with ARC extra)
typedef __unsafe TObject* PlainObjPtrT;
typedef __unsafe TComponent* PlainCompPtrT;
typedef __unsafe TRttiProperty* PlainRttiPropPtrT;
typedef __unsafe TRttiInstanceType* PlainRttiInstanceTypePtrT;
typedef __unsafe EsI18nManager* PlainEsI18nManagerPtrT;

/// EsI18nManager class - internatialization manager for GUI
///
class ESCORE_GUI_PKG EsI18nManager : public TComponent
{
protected:
	// Internal types
	//

	// Property node
	struct PropNode
	{
		PropNode(PlainRttiPropPtrT prop = nullptr) ES_NOTHROW;
		PropNode(const PropNode& other) ES_NOTHROW;
		bool operator==(const PropNode& other) const ES_NOTHROW;
		bool isStringList() const ES_NOTHROW;
		bool isString() const ES_NOTHROW;
		EsString nameGet() const ES_NOTHROW;
		EsString valueGet(PlainObjPtrT obj) const;
		void valueSet(PlainObjPtrT obj, const EsString& str) const;

		static bool isStringList(PlainRttiPropPtrT prop) ES_NOTHROW;
		static bool isString(PlainRttiPropPtrT prop) ES_NOTHROW;

		PlainRttiPropPtrT m_prop;
	};
	typedef std::map<PlainObjPtrT, PropNode> PropNodes;

	// Type|prop dictionary used for localizable items search and
	// original strings mapping creation
  //
	struct SearchDictionaryNode
	{
		PlainRttiInstanceTypePtrT m_cls;
		PropNodes m_props;
	};
	typedef std::map<PlainRttiInstanceTypePtrT, SearchDictionaryNode> SearchNodes;

	// Original strings mapping payload node
	struct OriginalMappingNode
	{
		PlainObjPtrT m_obj;
		PropNode m_pnode;

		OriginalMappingNode();
		OriginalMappingNode(PlainObjPtrT obj, const PropNode& pnode);
		OriginalMappingNode(const OriginalMappingNode& other);
		bool operator== (const OriginalMappingNode& other) const;

		void valueSet(const EsString& str) const;
	};

	typedef std::multimap<PlainObjPtrT, OriginalMappingNode> Object2OrgNodes;
	typedef std::pair<EsString, Object2OrgNodes> OrgStringObject2OrgNodes;
	typedef std::shared_ptr<OrgStringObject2OrgNodes> OrgStringObject2OrgNodesPtr;
	typedef std::map<EsString::HashT, OrgStringObject2OrgNodesPtr> OrgString2OrgNodePtrs;
	typedef std::multimap<PlainObjPtrT, EsString::HashT> Object2OrgStrings;

public:
	__fastcall EsI18nManager(TComponent* owner, const EsString& domain = EsString::null());
	virtual __fastcall ~EsI18nManager();

	/// Add translatable properties search criteria. multiple calls may be chained.
	/// propNames parameter is comma-separated string of translatable property names
	///
	EsI18nManager& i18nSearchCriterionAdd(const UnicodeString& cls, const EsString& propNames, bool reset = false);
	EsI18nManager& i18nSearchCriterionAdd(TMetaClass* cls, const EsString& propNames, bool reset = false);

	/// (Re)build key strings map (objects && text properties mapped to en-originals)
	///
	void orgStringsMapBuild(PlainObjPtrT obj = nullptr, bool nested = true, bool reset = false);

	/// Remove all specified object entries from I18N manager. This method is for
	/// dynamic object handling case, when certain stuff may be created|
	/// deleted during manager lifetime. To avoid AVs on attempts to update
	/// strings for non-existent objects, this method should be called when such dynamic
	/// object is deallocated.
	void onObjectRemoved(PlainObjPtrT obj);

	/// Return translation for the orgstring, or orgsting itself.
	/// If special dynamic string is used, parse it, compose translations, and remove markup
	EsString translationGet(const EsString& orgstr);

	/// Save original strings to the UTF-8 text file, in current directory.
	/// File name is formed from the domain passed to ctor+pot extension. If domain
	/// name is left by-default (empty string), the owning object name is used instead
	///
	void orgStringsExport() const;

	/// Pluggable onGuiStringsUpdate event handler. Called at the end of guiStringsUpdate
	///
	__property TNotifyEvent onGuiStringsUpdate = {read=m_onGuiStringsUpdate, write=m_onGuiStringsUpdate};

	/// Services - EsStringI18n delegates
	///

	/// Assign localization files search path
	///
	static void translationSearchPathSet(const EsString& searchPath);

	/// Assign localization files search paths
	///
	static void translationSearchPathsSet(const EsStringArray& searchPaths);

	/// Get default (fallback) locale id
	///
	static const EsString& defaultLocaleGet();

	/// Set default (fallback) locale id
	///
	static void defaultLocaleSet(const EsString& def);

	/// Return locales available for i18n, by searching and
	/// enumerating translation files in search path list.
	/// fallback locale set in defaultLocaleSet call is
	/// always available, even if no translation files were enumerated
	/// for it
	///
	static const EsString::Array& availableLocalesGet();

	/// Return currently active translation locale id
	///
	static const EsString& currentLocaleGet();

	/// Set currently active translation locale id
	/// If locale id differs from default one, all translation files, found in
	/// translation search paths list for this locale, will be loaded.
	/// If the locale being set is the same as default locale, all
	/// non-defaule translations will be unloaded from translation memory.
	///
	void currentLocaleSet(const EsString& loc);

  /// Return sinleton instance of this manager
  static PlainEsI18nManagerPtrT instanceGet() ES_NOTHROW;

protected:
	/// Internal services
	///

  /// Subscribed notification override
  virtual void __fastcall Notification(TComponent* AComponent, TOperation Operation);

	/// Build string search dictionary, given subjects
	///
	void searchDictionaryBuild(const EsString& subjects);

	/// Return search dictionary node for specified object or NULL if not found
	///
	const SearchDictionaryNode* dictionaryNodeForObjectGet(PlainObjPtrT obj);

	/// Append entry in obj2orgstrings lookup map
	///
	void orgStringsEntryAdd(PlainObjPtrT obj, EsString::HashT hash);

	/// Update original string map entry
	///
	void orgMapUpdate(const EsString& str, PlainObjPtrT, const PropNode& pnode);

	/// Return original string map payload
	///
	OrgStringObject2OrgNodesPtr orgMapNodesGet(const EsString& str) const;

	/// Scan object for localizable string properties, and build loclizable properties map
	///
	void objScan(PlainObjPtrT obj, bool nested);

	/// Scan object instance properties for localizable items
	///
	void objPropsScan(PlainObjPtrT obj, bool nested);

	/// GUI translation helper. Looks-up and applies (if found) translated string to mapped properties
	void translationLookupAndApply(const EsString& orgstr, const EsI18nManager::Object2OrgNodes& nodes);

	/// Force GUI strings update in accordance to the currently selected locale
	void guiStringsUpdate();

	/// Force object's GUI strings update in accordance to the currently selected locale
	void guiStringsUpdate(PlainObjPtrT obj);

	/// Link i18n translation scope
	void scopeLink(EsI18nManagerScope* scope);

	/// Unlink i18n translation scope
	void scopeUnlink(EsI18nManagerScope* scope);

  /// Singleton instance access
  static PlainEsI18nManagerPtrT& instanceAccess() ES_NOTHROW;

private:
	// data members
	//
	TNotifyEvent m_onGuiStringsUpdate;
	// managed gettext domain
	EsString m_domain;
	// RTTI context
	TRttiContext m_rtti;
	// localizable strings search dictionary
	SearchNodes m_search;
	// original strings mapping (keys)
	OrgString2OrgNodePtrs m_orgStrings;
	// Object-to-original strings mapping
	Object2OrgStrings m_obj2orgStrings;
	// Linked translation scopes
	typedef std::set<EsI18nManagerScope*> Scopes;
	Scopes m_scopes;
	// temporary set of looked-up objects, to prevent
	// objScan re-looping
	EsObjectSet m_lookedUpObjs;
	// compound markup parser
	EsRegEx m_re;
	// i18n scope under construction
	EsI18nManagerScope* m_scope;

	ES_NO_DEFAULT_CTR(EsI18nManager);
	ES_NON_COPYABLE(EsI18nManager);

	friend class EsI18nManagerScope;
};

/// I18N helper class - slave translation scope for dynamic objects
///
class ESCORE_GUI_CLASS EsI18nManagerScope
{
public:
	EsI18nManagerScope( PlainCompPtrT subj, PlainEsI18nManagerPtrT mgr = nullptr );
	~EsI18nManagerScope();

	/// Return true if scope is orphaned, i.e. not attached to I18N manager
	bool isOrphan() const ES_NOTHROW { return nullptr == m_mgr; }

	/// Pluggable onGuiStringsUpdate event handler. Called at the end of guiStringsUpdate
	///
	__property TNotifyEvent onGuiStringsUpdate = {read=m_onGuiStringsUpdate, write=m_onGuiStringsUpdate};

	/// (Re-) attach scope to I18N manager. If mgr is not 0, org strings map building
	/// is performed internally, and GUI strings got update in accordance with locale
	/// currently set in I18N manager
	void managerSet(PlainEsI18nManagerPtrT mgr, bool bindProps = true);
	/// Return pointer to attached I18N manager
	inline PlainEsI18nManagerPtrT managerGet() const { return m_mgr; }
	/// Rebuild scoped original strings mapping
	void orgStringsMapRebuild(bool reset = true);
	/// Force GUI strings update for this scope's subject
	void guiStringsUpdate();

protected:
	void objectsRemoved();
	void objectReferenceAdd(PlainObjPtrT obj);

protected:
	TNotifyEvent m_onGuiStringsUpdate;
	PlainEsI18nManagerPtrT m_mgr;
	PlainCompPtrT m_subj;
	EsObjectSet m_scopedObjs;
  bool m_bindProps;

  ES_NO_DEFAULT_CTR(EsI18nManagerScope);
	ES_NON_COPYABLE(EsI18nManagerScope);

	friend class EsI18nManager;
};

//---------------------------------------------------------------------------
#endif
