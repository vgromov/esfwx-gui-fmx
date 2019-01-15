#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsI18nManager.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

// internal service. InheritsFrom does not work properly in C++ Builder (as many other Delphi RTTI stuff)
static bool isInheritedFrom(PlainRttiInstanceTypePtrT t1, PlainRttiInstanceTypePtrT t2) ES_NOTHROW
{
	if( t1 && t2 )
	{
		while( t1 )
		{
			if( t1 == t2 )
				return true;

			t1 = static_cast<PlainRttiInstanceTypePtrT>(t1->BaseType);
		}
	}

	return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// property node
EsI18nManager::
PropNode::PropNode(PlainRttiPropPtrT prop /*= 0*/) ES_NOTHROW :
m_prop(prop)
{}
//---------------------------------------------------------------------------

EsI18nManager::
PropNode::PropNode(const PropNode& other) ES_NOTHROW :
m_prop(other.m_prop)
{}
//---------------------------------------------------------------------------

bool EsI18nManager::
PropNode::operator==(const PropNode& other) const ES_NOTHROW
{
	return m_prop == other.m_prop;
}
//---------------------------------------------------------------------------

bool EsI18nManager::
PropNode::isStringList(PlainRttiPropPtrT prop) ES_NOTHROW
{
	if( prop )
	{
		TRttiInstanceType* thisType = dynamic_cast<TRttiInstanceType*>(prop->PropertyType);
		TRttiContext rtti;
		TRttiInstanceType* stringsType = dynamic_cast<TRttiInstanceType*>(rtti.GetType(__classid(TStrings)));
		return isInheritedFrom(thisType, stringsType);
	}

	return false;
}
//---------------------------------------------------------------------------

bool EsI18nManager::
PropNode::isString(PlainRttiPropPtrT prop) ES_NOTHROW
{
	if( prop )
	{
		TTypeKind kind = prop->PropertyType->TypeKind;
		return TTypeKind::tkString == kind || TTypeKind::tkLString == kind ||
			TTypeKind::tkWString == kind || TTypeKind::tkUString == kind;
	}

	return false;
}
//---------------------------------------------------------------------------

bool EsI18nManager::
PropNode::isStringList() const ES_NOTHROW
{
	return isStringList(m_prop);
}
//---------------------------------------------------------------------------

bool EsI18nManager::
PropNode::isString() const ES_NOTHROW
{
	return isString(m_prop);
}
//---------------------------------------------------------------------------

EsString EsI18nManager::
PropNode::nameGet() const ES_NOTHROW
{
	if( m_prop )
		return m_prop->Name.c_str();

	return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsI18nManager::
PropNode::valueGet(PlainObjPtrT obj) const
{
	if( m_prop && obj )
	{
		TValue val = m_prop->GetValue(obj);
		if( isString() )
			return val.AsString().c_str();
		else
		{
			TStrings* strings = dynamic_cast<TStrings*>(val.AsObject());
			if(strings)
			{
				UnicodeString lb = strings->LineBreak;
				strings->LineBreak = esT("\n");
				const EsString& tmp = strings->Text.c_str();
				strings->LineBreak = lb;

				return tmp;
			}
		}
	}

	return EsString::null();
}
//---------------------------------------------------------------------------

void EsI18nManager::
PropNode::valueSet(PlainObjPtrT obj, const EsString& str) const
{
	if( m_prop && obj )
	{
		UnicodeString us = str.c_str();
		if( isString() )
		{
			const TValue& val = TValue::_op_Implicit(us);
			m_prop->SetValue(obj, val);
		}
		else
		{
			TValue val = m_prop->GetValue(obj);
			TStrings* strings = dynamic_cast<TStrings*>(val.AsObject());
			if(strings)
				strings->Text = us;
		}
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// original mapping node implementation
EsI18nManager::
OriginalMappingNode::OriginalMappingNode() :
m_obj(0)
{}
//---------------------------------------------------------------------------

EsI18nManager::
OriginalMappingNode::OriginalMappingNode(PlainObjPtrT obj, const PropNode& pnode) :
m_obj(obj),
m_pnode(pnode)
{}
//---------------------------------------------------------------------------

EsI18nManager::
OriginalMappingNode::OriginalMappingNode(const OriginalMappingNode& other) :
m_obj(other.m_obj),
m_pnode(other.m_pnode)
{}
//---------------------------------------------------------------------------

bool EsI18nManager::
OriginalMappingNode::operator == (const OriginalMappingNode& other) const
{
	return m_obj == other.m_obj && m_pnode == other.m_pnode;
}
//---------------------------------------------------------------------------

void EsI18nManager::
OriginalMappingNode::valueSet(const EsString& str) const
{
	m_pnode.valueSet(m_obj, str);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// EsI18nManager implementation
__fastcall EsI18nManager::EsI18nManager(TComponent* owner, const EsString& domain /*= EsString::null()*/) :
TComponent(owner),
m_onGuiStringsUpdate(nullptr),
m_domain(domain),
m_re(esT("#{2}([^#]+)")),
m_scope(nullptr)
{
  ES_ASSERT( nullptr == instanceAccess() );
  instanceAccess() = this;
}
//---------------------------------------------------------------------------

__fastcall EsI18nManager::~EsI18nManager()
{
  ES_DEBUG_TRACE(esT("EsI18nManager::~EsI18nManager"));

	while( !m_scopes.empty() )
		scopeUnlink( *m_scopes.begin() );

  instanceAccess() = nullptr;
}
//---------------------------------------------------------------------------

PlainEsI18nManagerPtrT& EsI18nManager::instanceAccess() ES_NOTHROW
{
  static PlainEsI18nManagerPtrT s_inst = nullptr;

  return s_inst;
}
//---------------------------------------------------------------------------

PlainEsI18nManagerPtrT EsI18nManager::instanceGet() ES_NOTHROW
{
  return instanceAccess();
}
//---------------------------------------------------------------------------

EsI18nManager& EsI18nManager::i18nSearchCriterionAdd(const UnicodeString& cls, const EsString& propNames, bool reset /*= false*/)
{
  TMetaClass* pcls = FindClass(cls);
  return i18nSearchCriterionAdd(pcls, propNames, reset);
}
//---------------------------------------------------------------------------

EsI18nManager& EsI18nManager::i18nSearchCriterionAdd(TMetaClass* cls, const EsString& propNames, bool reset /*=false*/)
{
	ES_ASSERT(cls);
	ES_ASSERT(!propNames.empty());

	// reset search dictionary
	if( reset )
		m_search.clear();

	TRttiInstanceType* type = dynamic_cast<TRttiInstanceType*>(m_rtti.GetType(cls));
	ES_ASSERT(type);
#ifdef ES_DEBUG
	UnicodeString tn = type->Name;
#endif

	EsStringTokenizer tok(propNames, ES_COMMA);
	PropNodes props;
	while( tok.get_moreTokens() )
	{
		const EsString& propName = tok.get_nextToken();
		TRttiProperty* prop = type->GetProperty(propName.c_str());
		if( prop && (mvPublic == prop->Visibility || mvPublished == prop->Visibility) &&
				prop->IsReadable && prop->IsWritable &&
				(PropNode::isString(prop) || PropNode::isStringList(prop)) )
		{
#ifdef ES_DEBUG
			UnicodeString pn = prop->Name;
#endif
			props[prop] = PropNode(prop);
		}
	}

	if( !props.empty() )
	{
		SearchDictionaryNode node;
		node.m_cls = type;
		node.m_props = props;
		m_search[type] = node;
	}

	return *this;
}
//---------------------------------------------------------------------------

template <typename SearchNodesT>
struct TypeFinder
{
	TypeFinder(TRttiInstanceType* subj) :
	m_subj(subj)
	{}

	bool operator() (typename SearchNodesT::value_type& obj)
	{
		return isInheritedFrom(m_subj, obj.first);
	}

	TRttiInstanceType* m_subj;
};
//---------------------------------------------------------------------------

void EsI18nManager::orgStringsEntryAdd(PlainObjPtrT obj, EsString::HashT hash)
{
	m_obj2orgStrings.insert( Object2OrgStrings::value_type(obj, hash) );

	if(m_scope)
		m_scope->objectReferenceAdd(obj);
}
//---------------------------------------------------------------------------

void EsI18nManager::orgMapUpdate(const EsString& str, PlainObjPtrT obj, const EsI18nManager::PropNode& pnode)
{
	OrgStringObject2OrgNodesPtr porgStr2Nodes = orgMapNodesGet(str);
	OriginalMappingNode node(obj, pnode);
	EsString::HashT hash = str.hashGet();

	if( porgStr2Nodes )
	{
		bool foundExistingOrgNode = false;
		const Object2OrgNodes& orgNodes = porgStr2Nodes->second;
		const std::pair<Object2OrgNodes::const_iterator,
										Object2OrgNodes::const_iterator>& orgNodesRange =
			orgNodes.equal_range(obj);

		for(Object2OrgNodes::const_iterator cit = orgNodesRange.first;
				cit != orgNodesRange.second; ++cit)
		{
			if( (*cit).second == node )
			{
				foundExistingOrgNode = true;
				break;
			}
		}

		if( !foundExistingOrgNode )
		{
			porgStr2Nodes->second.insert( Object2OrgNodes::value_type(obj, node) );

			orgStringsEntryAdd(obj, hash);
		}
	}
	else
	{
		porgStr2Nodes = OrgStringObject2OrgNodesPtr(new OrgStringObject2OrgNodes);
		ES_ASSERT(porgStr2Nodes);
		porgStr2Nodes->first = str;
		porgStr2Nodes->second.insert( Object2OrgNodes::value_type(obj, node) );
		m_orgStrings[hash] = porgStr2Nodes;

		orgStringsEntryAdd(obj, hash);
	}
}
//---------------------------------------------------------------------------

// return original string map payload
EsI18nManager::OrgStringObject2OrgNodesPtr EsI18nManager::orgMapNodesGet(const EsString& str) const
{
	OrgString2OrgNodePtrs::const_iterator cit = m_orgStrings.find(str.hashGet());
	if( cit != m_orgStrings.end() )
	{
		ES_ASSERT( (*cit).second->first == str );
		return (*cit).second;
	}

	return OrgStringObject2OrgNodesPtr();
}
//---------------------------------------------------------------------------

// scan object for localizable string properties, and build localizable properties map
void EsI18nManager::objScan(PlainObjPtrT obj, bool nested)
{
	if( m_search.empty() )
    return;

	ES_ASSERT( obj );

	if( !m_lookedUpObjs.insert(obj).second )
		return;

#ifdef ES_DEBUG
	UnicodeString tn = obj->ClassName();
#endif

	const SearchDictionaryNode* search = dictionaryNodeForObjectGet(obj);
	if( search )
	{
		for(PropNodes::const_iterator cit = search->m_props.begin(); cit != search->m_props.end(); ++cit)
		{
			const PropNode& propNode = (*cit).second;
			const EsString& str = propNode.valueGet(obj);
#ifdef ES_DEBUG
			UnicodeString pn = propNode.nameGet().c_str();
#endif
			if( !str.empty() )
				orgMapUpdate(str, obj, propNode);
		}
	}

  TComponent* fobj = dynamic_cast<TComponent*>(obj);
  if( fobj )
  {
    // Register free notification
    fobj->FreeNotification(this);

    // Scan children objects, if any
    if( nested )
    {
      for(int idx = 0; idx < fobj->ComponentCount; ++idx)
        objScan(fobj->Components[idx], nested);
    }
  }

	// Scan object instance properties|indexed instance properties
	objPropsScan(obj, nested);
}
//---------------------------------------------------------------------------

/// scan object instance properties for localizable items
///
void EsI18nManager::objPropsScan(PlainObjPtrT obj, bool nested)
{
	TRttiInstanceType* type = dynamic_cast<TRttiInstanceType*>(m_rtti.GetType( obj->ClassType() ));
	if( type )
	{
		// enumerate simple props
		const DynamicArray<TRttiProperty*>& props = type->GetProperties();
		for(int idx = 0; idx < props.Length; ++idx)
		{
			TRttiProperty* prop = props[idx];
			TRttiType* pt = prop->PropertyType;
			const EsString& pname = prop->Name.c_str();

			if( esT("Parent") == pname ||
					esT("Owner") == pname ||
					esT("Collection") == pname ||
					esT("Action") == pname )
				continue;

			if( pt->IsInstance && mvPublished == prop->Visibility && prop->IsReadable )
			{
				TValue val = prop->GetValue(obj);
				TObject* pobj = val.AsObject();
				if( pobj )
					objScan(pobj, nested);
			}
		}

		// enumerate indexed props
		const DynamicArray<TRttiIndexedProperty*>& iprops = type->GetIndexedProperties();
		for(int idx = 0; idx < iprops.Length; ++idx)
		{
			TRttiIndexedProperty* prop = iprops[idx];
			const EsString& pname = prop->Name.c_str();
			if( esT("Items") == pname )
			{
				TRttiProperty* pcount = type->GetProperty(esT("Count"));
				if( pcount )
				{
					TRttiType* pt = prop->PropertyType;
					if( pt->IsInstance &&
							(mvPublished == prop->Visibility || mvPublic == prop->Visibility) &&
							prop->IsReadable )
					{
						TValue cnt = pcount->GetValue(obj);
						int count = cnt.AsInteger();

						for(int iidx = 0; iidx < count; ++iidx)
						{
							TValue vidx = TValue::_op_Implicit(iidx);
							TValue val = prop->GetValue(obj, &vidx, 0); //< 0 in arg size is due to Delphi array parameters specifics in C++
							TObject* pobj = val.AsObject();
							if( pobj )
								objScan(pobj, nested);
						}
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

// return search dictionary node for specified object or NULL, if not found
const EsI18nManager::SearchDictionaryNode* EsI18nManager::dictionaryNodeForObjectGet(PlainObjPtrT obj)
{
	if( obj && !m_search.empty() )
	{
		TRttiInstanceType* type = dynamic_cast<TRttiInstanceType*>(m_rtti.GetType( obj->ClassType() ));
		ES_ASSERT(type);
		TypeFinder<SearchNodes> finder(type);
		SearchNodes::const_iterator cit = std::find_if(m_search.begin(), m_search.end(), finder);
		if( cit != m_search.end() )
			return &(*cit).second;
	}

	return 0;
}
//---------------------------------------------------------------------------

// (re)build key strings map (objects && text properties mapped to en-originals)
//
void EsI18nManager::orgStringsMapBuild(PlainObjPtrT obj /*= nullptr*/, bool nested /*= true*/, bool reset /*= false*/)
{
	m_lookedUpObjs.clear();

	if( reset )
	{
		m_orgStrings.clear();
		m_obj2orgStrings.clear();
	}

	if( nullptr == obj )
		obj = Owner;

	try
	{
		objScan(obj, nested);
	}
	__finally
	{
		m_lookedUpObjs.clear();
	}
}
//---------------------------------------------------------------------------

/// Save original strings to the UTF-8 text file, in current directory.
/// File name is formed from the domain passed to ctor+pot extension. If domain
/// name is left by-default (empty string), the owning object name is used instead
///
void EsI18nManager::orgStringsExport() const
{
	if( !m_orgStrings.empty() )
	{
		ES_ASSERT(Owner);
		const EsString& fname = m_domain.empty() ? EsString(Owner->Name.c_str()) :
			m_domain;

		EsFile f(
      fname + esT(".GUI.pot"),
      static_cast<ulong>(EsFileFlag::Text)|static_cast<ulong>(EsFileFlag::Write)
    );

		if( f.open() )
		{
			f.stringWrite(esT("msgid \"\"\nmsgstr \"\"\n"));
			f.stringWrite(esT("\"Content-Type: text/plain; charset=UTF-8\\n\"\n"));
			f.stringWrite(esT("\"Content-Transfer-Encoding: 8bit\\n\"\n\n"));

			for(OrgString2OrgNodePtrs::const_iterator cit = m_orgStrings.begin();
					cit != m_orgStrings.end(); ++cit )
			{
				const EsString& str = EsString::format(
          esT("msgid \"%s\"\nmsgstr \"\"\n\n"),
					EsString::toString( (*cit).second->first )
        );
				f.stringWrite(str);
			}

			f.close();
		}
	}
}
//---------------------------------------------------------------------------

static inline bool isSpecialMarkup(const EsString& orgstr) ES_NOTHROW
{
	size_t sze = orgstr.size();
	return sze > 4 &&
		esT('#') == orgstr[0] && esT('#') == orgstr[1] &&
		esT('#') == orgstr[sze-2] && esT('#') == orgstr[sze-1];
}
//---------------------------------------------------------------------------

EsString EsI18nManager::translationGet(const EsString& orgstr)
{
	if( !isSpecialMarkup(orgstr) )
		return EsStringI18n::instanceGet().translationGet(orgstr, m_domain);
	else
	{
		EsString result;
		m_re.set_text(orgstr);
		while( m_re.get_matches() )
		{
			ulong start, len;
			if( m_re.matchGet(start, len, 1) )
			{
				EsString::const_iterator beg = orgstr.begin()+start;
				EsString::const_iterator end = beg+len;
				EsString part(beg, end);
				part = EsStringI18n::instanceGet().translationGet(part, m_domain);

				result.append( part );
				m_re.set_offset(start+len);
			}
			else
				break;
		}

		return result;
	}
}
//---------------------------------------------------------------------------

void EsI18nManager::translationLookupAndApply(const EsString& orgstr,	const EsI18nManager::Object2OrgNodes& nodes)
{
	// retrieve string translation for the original
	const EsString& str =	translationGet(orgstr);
	for(Object2OrgNodes::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
	{
		const OriginalMappingNode& node = (*cit).second;
		node.valueSet(str);
	}
}
//---------------------------------------------------------------------------

/// force GUI strings update in accordance to the currently selected locale
///
void EsI18nManager::guiStringsUpdate()
{
	for(OrgString2OrgNodePtrs::const_iterator cit = m_orgStrings.begin();
			cit != m_orgStrings.end(); ++cit )
	{
		const EsString& org = (*cit).second->first;
		const Object2OrgNodes& nodes = (*cit).second->second;

		translationLookupAndApply(org, nodes);
	}

	if( m_onGuiStringsUpdate )
		m_onGuiStringsUpdate(this);

	for(Scopes::const_iterator sit = m_scopes.begin(); sit != m_scopes.end(); ++sit)
		if( (*sit)->m_onGuiStringsUpdate )
			(*sit)->m_onGuiStringsUpdate(this);
}
//---------------------------------------------------------------------------

/// Force object's GUI strings update in accordance to the currently selected locale
void EsI18nManager::guiStringsUpdate(PlainObjPtrT obj)
{
	std::pair<Object2OrgStrings::const_iterator,
						Object2OrgStrings::const_iterator> orgstrRange = m_obj2orgStrings.equal_range(obj);

	for(Object2OrgStrings::const_iterator cit = orgstrRange.first;
			cit != orgstrRange.second; ++cit)
	{
		OrgString2OrgNodePtrs::const_iterator ccit = m_orgStrings.find( (*cit).second );
		if( ccit != m_orgStrings.end() )
		{
			const EsString& org = (*ccit).second->first;
			const Object2OrgNodes& nodes = (*ccit).second->second;

			translationLookupAndApply(org, nodes);
		}
	}

	if( m_onGuiStringsUpdate )
		m_onGuiStringsUpdate(this);
}
//---------------------------------------------------------------------------

void __fastcall EsI18nManager::Notification(TComponent* AComponent, TOperation Operation)
{
  if( TOperation::opRemove == Operation )
  {
    ES_ASSERT(AComponent);

    onObjectRemoved( AComponent );
  }

  TComponent::Notification(AComponent, Operation);
}
//---------------------------------------------------------------------------

void EsI18nManager::onObjectRemoved(PlainObjPtrT obj)
{
	// first, find object record in m_obj2orgStrings
	std::pair< Object2OrgStrings::const_iterator,
		Object2OrgStrings::const_iterator > orgstrRange = m_obj2orgStrings.equal_range(obj);

	// for each entry in selected range, find and erase object records in m_orgStrings
	for( Object2OrgStrings::const_iterator cit = orgstrRange.first; cit != orgstrRange.second; ++cit )
	{
		OrgString2OrgNodePtrs::const_iterator ccit = m_orgStrings.find( (*cit).second );
		if( ccit != m_orgStrings.end() )
		{
#ifdef ES_DEBUG
			size_t cntRemoved =
#endif
			(*ccit).second->second.erase(obj);
		}
	}

	// finally, erase object records in m_obj2orgStrings
  m_obj2orgStrings.erase(obj);
}
//---------------------------------------------------------------------------

void EsI18nManager::translationSearchPathSet(const EsString& searchPath)
{
  EsStringTokenizer tok(searchPath, esT(";\n\r"));
  size_t cnt = tok.get_tokensCount();

	EsStringArray paths;
  paths.reserve(cnt);
  while( tok.get_moreTokens() )
    paths.push_back( tok.get_nextToken() );

	translationSearchPathsSet(paths);
}
//---------------------------------------------------------------------------

void EsI18nManager::translationSearchPathsSet(const EsStringArray& searchPaths)
{
	EsStringI18n::instanceGet().translationSearchPathsSet(searchPaths);
}
//---------------------------------------------------------------------------

/// get default (fallback) locale id
///
const EsString& EsI18nManager::defaultLocaleGet()
{
	return EsStringI18n::instanceGet().defaultLocaleGet();
}
//---------------------------------------------------------------------------

/// set default (fallback) locale id
///
void EsI18nManager::defaultLocaleSet(const EsString& def)
{
	EsStringI18n::instanceGet().defaultLocaleSet(def);
}
//---------------------------------------------------------------------------

/// return locales available for i18n, by searching and
/// enumerating translation files in search path list.
/// fallback locale set in defaultLocaleSet call is
/// always available, even if no translation files were enumerated
/// for it
///
const EsString::Array& EsI18nManager::availableLocalesGet()
{
	return EsStringI18n::instanceGet().availableLocalesGet();
}
//---------------------------------------------------------------------------

/// return currently active translation locale id
///
const EsString& EsI18nManager::currentLocaleGet()
{
	return EsStringI18n::instanceGet().currentLocaleGet();
}
//---------------------------------------------------------------------------

/// Set currently active translation locale id
/// If locale id differs from default one, all translation files, found in
/// translation search paths list for this locale, will be loaded.
/// If the locale being set is the same as default locale, all
/// non-defaule translations will be unloaded from translation memory.
///
void EsI18nManager::currentLocaleSet(const EsString& loc)
{
	EsStringI18n::instanceGet().currentLocaleSet(loc);
	guiStringsUpdate();
}
//---------------------------------------------------------------------------

/// Link i18n translation scope
void EsI18nManager::scopeLink(EsI18nManagerScope* scope)
{
	if( scope && m_scopes.end() == m_scopes.find(scope) )
	{
		ES_ASSERT(scope->m_mgr == 0);
		scope->m_mgr = this;
		m_scopes.insert(scope);
	}
}
//---------------------------------------------------------------------------

/// Unlink i18n translation scope
void EsI18nManager::scopeUnlink(EsI18nManagerScope* scope)
{
	if( scope && m_scopes.end() != m_scopes.find(scope) )
	{
		ES_ASSERT(scope->m_mgr == this);
		scope->objectsRemoved();
		scope->m_mgr = 0;
		m_scopes.erase(scope);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
EsI18nManagerScope::EsI18nManagerScope( PlainCompPtrT subj, PlainEsI18nManagerPtrT mgr /*= 0*/  ) :
m_mgr(mgr),
m_subj(subj),
m_bindProps(true)
{
	ES_ASSERT(m_subj);
	ES_ASSERT(m_subj != m_mgr);

	if(m_mgr)
    m_mgr->scopeLink(this);
}
//---------------------------------------------------------------------------

EsI18nManagerScope::~EsI18nManagerScope()
{
	managerSet(nullptr);
}
//---------------------------------------------------------------------------

void EsI18nManagerScope::objectReferenceAdd(PlainObjPtrT obj)
{
	ES_ASSERT(obj);
	ES_ASSERT(m_mgr);
	ES_ASSERT(m_mgr->m_scope == this);
	if( obj != m_subj )
		m_scopedObjs.insert(obj);
}
//---------------------------------------------------------------------------

void EsI18nManagerScope::objectsRemoved()
{
	ES_ASSERT( m_mgr );
	for( EsObjectSet::const_iterator cit = m_scopedObjs.begin(); cit != m_scopedObjs.end(); ++cit )
		m_mgr->onObjectRemoved( (*cit) );
  m_scopedObjs.clear();
	m_mgr->onObjectRemoved(m_subj);
}
//---------------------------------------------------------------------------

void EsI18nManagerScope::orgStringsMapRebuild(bool reset /*= true*/)
{
	if( m_mgr )
	{
		if( reset )
			objectsRemoved();

    if( m_bindProps )
    {
      EsI18nManagerScope* oldScope = m_mgr->m_scope;
      m_mgr->m_scope = this;

      try
      {
        m_mgr->orgStringsMapBuild(m_subj);
      }
      __finally
      {
        m_mgr->m_scope = oldScope;
      }
    }
	}
}
//---------------------------------------------------------------------------

void EsI18nManagerScope::managerSet(PlainEsI18nManagerPtrT mgr, bool bindProps /*= true*/)
{
	if( m_mgr != mgr )
	{
		if( m_mgr )
			m_mgr->scopeUnlink(this);

    m_bindProps = bindProps;

		if( mgr ) // internally, scopeLink will assign mgr to our m_mgr member
		{
			mgr->scopeLink(this);
			orgStringsMapRebuild();
			guiStringsUpdate();
		}
	}
}
//---------------------------------------------------------------------------

void EsI18nManagerScope::guiStringsUpdate()
{
	if( m_mgr )
	{
		for( EsObjectSet::const_iterator cit = m_scopedObjs.begin(); cit != m_scopedObjs.end(); ++cit )
			m_mgr->guiStringsUpdate( (*cit) );

		m_mgr->guiStringsUpdate(m_subj);

		if( m_onGuiStringsUpdate )
			m_onGuiStringsUpdate(m_mgr);
	}
}
//---------------------------------------------------------------------------

