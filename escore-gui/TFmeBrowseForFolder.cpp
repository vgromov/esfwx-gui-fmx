#include "escore_gui_pch.h"
#pragma hdrstop

#include "TFmeBrowseForFolder.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

TFmeBrowseForFolder::
Subscriber::Subscriber(TFmeBrowseForFolder& owner) :
EsEventSubscriber(owner.ClassName().c_str()),
m_owner(owner)
{
}
//---------------------------------------------------------------------------

// event notification interface
void TFmeBrowseForFolder::Subscriber::notify(const EsEventIntf::Ptr& evt)
{
	ES_ASSERT(evt);
	const EsVariant::Array& payload = evt->payloadGet().asVariantCollection();
	switch( evt->idGet() )
	{
	case evtFolderListingUpdated:
		if( (void*)&m_owner == payload[0].asPointer() )
		{
			evt->consume();
			m_owner.listingSet(
        payload[1].asStringCollection(),
        EsString::nullArray()
      );
		}
		break;
	case evtFilesListingUpdated:
		if( (void*)&m_owner == payload[0].asPointer() )
		{
			evt->consume();
			m_owner.listingSet(
        payload[1].asStringCollection(),
        payload[2].asStringCollection()
      );
		}
		break;
	}
}

//---------------------------------------------------------------------------
__fastcall TFmeBrowseForFolder::TFmeBrowseForFolder(TComponent* Owner) :
TFrame(Owner),
m_evtSub(*this),
m_flags(0),
m_idxImgFolderUp(-1),
m_idxImgFolder(-1),
m_idxImgFile(-1),
m_onChange(0)
{
#ifdef ES_IS_MOBILE_OS
  layCurFolder_->Height = 50;
  layCurFile_->Height = 50;
#else
  layCurFolder_->Height = 26;
  layCurFile_->Height = 26;
#endif

  // Initially, hide files controls
  layCurFile_->Parent = 0;
  layCurFile_->Visible = false;

  m_filePatt = esT(".*");
	rootFoldersSet(EsString::nullArray());
}
//---------------------------------------------------------------------------

__fastcall TFmeBrowseForFolder::~TFmeBrowseForFolder()
{}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::imagesSet(TCustomImageList* images)
{
  btnGotoHome_->Images = images;
  btnGotoHome_->ImageIndex =
    EsUtilities::imageIndexFindByName(images, esT("browseForFolderHome"));

  lbFolders_->Images = images;

  int oldIdxImgFolderUp = m_idxImgFolderUp;
  int oldIdxImgFolder = m_idxImgFolder;
  int oldIdxImgFile = m_idxImgFile;

  m_idxImgFolderUp = EsUtilities::imageIndexFindByName(images, esT("browseForFolderFolderUp"));
  m_idxImgFolder = EsUtilities::imageIndexFindByName(images, esT("browseForFolderFolder"));
  m_idxImgFile = EsUtilities::imageIndexFindByName(images, esT("browseForFolderFile"));

  EsUpdateLocker<TListBox> lock(lbFolders_);
  // Update existing list image indexes, if needed
  for( int idx = 0; idx < lbFolders_->Count; ++idx )
  {
    TListBoxItem* item = lbFolders_->ListItems[idx];
    int imgIdx = item->ImageIndex;
    if( -1 < imgIdx )
    {
      if( oldIdxImgFolderUp == imgIdx )
        imgIdx = m_idxImgFolderUp;
      else if( oldIdxImgFolder == imgIdx )
        imgIdx = m_idxImgFolder;
      else if( oldIdxImgFile == imgIdx )
        imgIdx = m_idxImgFile;

      item->ImageIndex = imgIdx;
    }
  }
}
//---------------------------------------------------------------------------

const EsString& TFmeBrowseForFolder::findRootForFolder(const EsString& folder) const
{
  for( size_t idx = 0; idx < m_rootFolders.size(); ++idx )
  {
    size_t pos = folder.find( m_rootFolders[idx] );
	  if( pos == 0 )
      return m_rootFolders[idx];
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

/// Return true if file item is selected in list
bool TFmeBrowseForFolder::isFileSelected() const
{
  return showFilesGet() &&
    !edCurFile_->Text.IsEmpty();
}
//---------------------------------------------------------------------------

/// Return true if folder item is selected in list
bool TFmeBrowseForFolder::isFolderSelected() const
{
  TListBoxItem* sel = lbFolders_->Selected;
  return sel && 0 == sel->Tag;
}
//---------------------------------------------------------------------------

/// Return true if home view is showing
bool TFmeBrowseForFolder::isHome() const
{
  return (m_flags & flagHomeView);
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::listingSet(const EsString::Array& folders,
  const EsString::Array& files)
{
  EsUpdateLocker<TListBox> lock(lbFolders_);
  lbFolders_->Clear();

  for(size_t idx = 0; idx < folders.size(); ++idx)
  {
    const EsString& f = folders[idx];
    if( esT(".") == f )
      continue;

    TListBoxItem* item = new TListBoxItem(lbFolders_);
    ES_ASSERT(item);

    item->Height = 28;
    item->Text = f.c_str();
    item->Tag = NativeInt(0);

    if( esT("..") == f )
    {
      item->Selectable = false;
      item->ImageIndex = m_idxImgFolderUp;
    }
    else
      item->ImageIndex = m_idxImgFolder;

    lbFolders_->AddObject(item);
  }

  for(size_t idx = 0; idx < files.size(); ++idx)
  {
    TListBoxItem* item = new TListBoxItem(lbFolders_);
    ES_ASSERT(item);

    item->Height = 28;
    item->Text = files[idx].c_str();
    item->Tag = NativeInt(1);
    item->ImageIndex = m_idxImgFile;

    lbFolders_->AddObject(item);
  }

  if( lbFolders_->Count )
    lbFolders_->ItemIndex = 0;
}
//---------------------------------------------------------------------------

const EsString& TFmeBrowseForFolder::currentFolderGet() const
{
	return m_currentFolder;
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::currentFolderSet(const EsString& folder)
{
  if( m_currentRoot.empty() )
    m_currentRoot = m_rootFolders[0];

  EsString newFolder = folder;
	if( folder.empty() )
    newFolder = m_currentRoot;

	if( m_currentFolder != newFolder )
	{
		if( !EsPath::dirExists(newFolder, EsString::null()) )
			EsException::Throw(
        esT("Could not set current folder to non-existent path: '%s'"),
				newFolder
      );

    // 1) find the appropriate root for folder
    const EsString& foundRoot = findRootForFolder(newFolder);
    if( foundRoot.empty() )
      gotoHome();
    else
    {
      m_currentRoot = foundRoot;
      m_currentFolder = newFolder;
      m_flags &= ~flagHomeView;
    }

		edCurFolder_->Text = m_currentFolder.c_str();

		currentFolderList();
	}
}
//---------------------------------------------------------------------------

const EsStringArray& TFmeBrowseForFolder::rootFoldersGet() const
{
	return m_rootFolders;
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::rootFoldersSet(const EsStringArray& rootFolders)
{
  EsStringArray newRoots = rootFolders;
	if( rootFolders.empty() )
    newRoots.push_back( EsPath::stdHomeGet() );

	if( m_rootFolders != newRoots )
	{
		m_rootFolders = newRoots;
    m_flags &= ~flagHomeView; //< Allow to refresh home view
    gotoHome();
	}
}
//---------------------------------------------------------------------------

class EsFolderLister : public EsPathEnumerator
{
public:
	EsFolderLister(const EsString& startPath, bool showGoToParent) :
	EsPathEnumerator(startPath),
	m_showGoToParent(showGoToParent)
	{
		m_folders.reserve(32);
	}

	const EsString::Array& foldersGet() const { return m_folders; }

protected:
	virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
	{
		if( isDir && name != esT(".") )
		{
			if( m_showGoToParent ||
					(!m_showGoToParent && name != esT("..")) )
				m_folders.push_back(name);
		}

		return true;
	}

private:
	EsString::Array m_folders;
	bool m_showGoToParent;
};
//---------------------------------------------------------------------------

class EsFileLister : public EsPathEnumerator
{
public:
	EsFileLister(const EsString& startPath, const EsString& patt) :
	EsPathEnumerator(startPath),
  m_re(patt, static_cast<ulong>(EsRegExCompileFlag::DEFAULT)|
             static_cast<ulong>(EsRegExCompileFlag::ICASE))
	{
		m_files.reserve(32);
	}

	const EsString::Array& filesGet() const { return m_files; }

protected:
	virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
	{
		if( !isDir )
		{
      m_re.set_text(name);
      if( m_re.get_matches() )
        m_files.push_back(name);
		}

		return true;
	}

private:
  EsRegEx m_re;
	EsString::Array m_files;
};
//---------------------------------------------------------------------------

bool TFmeBrowseForFolder::currentIsOneOfRootFolders() const
{
  ES_ASSERT( !m_currentFolder.empty() );
  ES_ASSERT( !m_rootFolders.empty() );

  for( size_t idx = 0; idx < m_rootFolders.size(); ++idx )
  {
    const EsString& folder = m_rootFolders[idx];
    if( folder == m_currentFolder )
      return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::currentFolderList()
{
  if( isHome() )
    return;

  if( IsUpdating() )
    m_flags |= flagListingPending;
  else
  {
    m_flags &= ~flagListingPending;

    EsFolderLister lister(m_currentFolder, !currentIsOneOfRootFolders());
    lister.execute();

    if( !(m_flags & flagShowFiles) )
    {
      EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
      payload
        .addToVariantCollection( EsVariant(this, EsVariant::ACCEPT_POINTER) /* NB! explicit convertion required*/ )
        .addToVariantCollection( lister.foldersGet() );

      EsEventDispatcher::eventPost(
        ClassName().c_str(),
        static_cast<ulong>(evtFolderListingUpdated),
        payload
      );
    }
    else
    {
      EsFileLister flister(m_currentFolder, m_filePatt);
      flister.execute();

      EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
      payload
        .addToVariantCollection( EsVariant(this, EsVariant::ACCEPT_POINTER) /* NB! explicit convertion required*/ )
        .addToVariantCollection( lister.foldersGet() )
        .addToVariantCollection( flister.filesGet() );

      EsEventDispatcher::eventPost(
        ClassName().c_str(),
        static_cast<ulong>(evtFilesListingUpdated),
        payload
      );
    }
  }
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::onFolderItemSelected(const TListBoxItem *item)
{
	if( item )
	{
    if( 0 == item->Tag )
    {
      if( isHome() )
      {
        currentFolderSet( const_cast<TListBoxItem*>(item)->Text.c_str() );
        if( m_onChange )
          m_onChange(this);
      }
      else
      {
        EsPath path = EsPath::createFromPath(m_currentFolder);
        ES_ASSERT( path.isOk() );

        // workaround the FM3 GetText missing constness bug
        if( const_cast<TListBoxItem*>(item)->Text == esT("..") )
          path.dirLastRemove();
        else
          path.dirAppend( const_cast<TListBoxItem*>(item)->Text.c_str() );

        EsString oldFolder = m_currentFolder;
        currentFolderSet( path.pathGet() );
        if( m_onChange && oldFolder != m_currentFolder )
          m_onChange(this);
      }
    }
    else if( 1 == item->Tag )
    {
      EsString newFile = const_cast<TListBoxItem*>(item)->Text.c_str();

      if( m_currentFile != newFile )
      {
        m_currentFile = newFile;

        if( m_onChange )
          m_onChange(this);
      }
    }
	}
}
//---------------------------------------------------------------------------

bool TFmeBrowseForFolder::showFilesGet() const
{
  return (m_flags & flagShowFiles);
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::showFilesSet( bool show )
{
  if( static_cast<bool>(m_flags & flagShowFiles) != show )
  {
    if( show )
    {
      m_flags |= flagShowFiles;
      layCurFile_->Visible = true;
      layCurFile_->Position->Y =
        layCurFolder_->Position->Y+
        layCurFolder_->Height+1;
      layCurFile_->Parent = this;
    }
    else
    {
      m_flags &= ~flagShowFiles;
      layCurFile_->Parent = 0;
      layCurFile_->Visible = false;
    }

    m_currentFile.clear();
    currentFolderList();
  }
}
//---------------------------------------------------------------------------

bool TFmeBrowseForFolder::flagFileNameEditableGet() const
{
  return (m_flags & flagFileNameEditable);
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::flagFileNameEditableSet( bool editable )
{
  if( static_cast<bool>(m_flags & flagFileNameEditable) != editable )
  {
    if( editable )
      m_flags |= flagFileNameEditable;
    else
      m_flags &= ~flagFileNameEditable;

    edCurFile_->ReadOnly = !editable;
  }
}
//---------------------------------------------------------------------------

const EsString& TFmeBrowseForFolder::filePatternGet() const
{
  return m_filePatt;
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::filePatternSet(const EsString& patt)
{
  EsString tmp;
  if( patt.empty() )
    tmp = esT(".*");
  else
    tmp = patt;

  if( m_filePatt != tmp )
  {
    m_filePatt = tmp;
    m_currentFile.clear();

    currentFolderList();
  }
}
//---------------------------------------------------------------------------

EsString TFmeBrowseForFolder::currentFileGet() const
{
  EsPath fpath = EsPath::createFromPath(m_currentFolder);

  fpath.fileNameSet( edCurFile_->Text.c_str() );

  return fpath.pathGet();
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::currentFileSet(const EsString& file)
{
  if( m_flags & flagShowFiles )
  {
    if( m_currentFile != file )
    {
      const EsPath& fpath = EsPath::createFromFilePath(file);
      const EsString& folder = fpath.pathGet(
          static_cast<ulong>(EsPathFlag::Default)|
          static_cast<ulong>(EsPathFlag::ExcludeFile),
          fpath.isRelative() ?
            m_currentFolder :
            EsString::null()
        );
      currentFolderSet( folder );
      const EsString& newFile = fpath.fileNameExtGet();
      if( m_currentFile != newFile )
      {
        m_currentFile = newFile;
        if(m_onChange)
          m_onChange(this);
      }
    }
  }
}
//---------------------------------------------------------------------------

void TFmeBrowseForFolder::gotoHome()
{
  if( !(m_flags & flagHomeView) )
  {
    m_flags |= flagHomeView;
    m_currentFile.clear();
    m_currentFolder.clear();
    edCurFile_->Text = EmptyStr;
    edCurFolder_->Text = EmptyStr;
    {
      EsUpdateLocker<TListBox> lock(lbFolders_);
      lbFolders_->Clear();
      for(int idx = 0; idx < static_cast<int>(m_rootFolders.size()); ++idx)
      {
        TListBoxItem* item = new TListBoxItem(lbFolders_);
        ES_ASSERT(item);
        item->Text = m_rootFolders[idx].c_str();
        lbFolders_->AddObject(item);
      }
    }
    lbFolders_->RealignContent();
  }
}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::EndUpdate()
{
  TFrame::EndUpdate();

  if( !IsUpdating() &&
      (m_flags & flagListingPending) )
    currentFolderList();
}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::lbFolders_ItemClick(const TCustomListBox *Sender,
					const TListBoxItem *Item)
{
  if( 1 == Item->Tag )
    edCurFile_->Text = const_cast<TListBoxItem*>(Item)->Text;
  else
    edCurFile_->Text = EmptyStr;

  if( m_flags & flagShowFiles ) // Dive into item immediately only if we're showing files
    onFolderItemSelected(Item);
}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::lbFolders_DblClick(TObject *Sender)
{
  if( -1 != lbFolders_->ItemIndex )
  {
    const TListBoxItem *item = lbFolders_->ListItems[lbFolders_->ItemIndex];
    ES_ASSERT(item);
    onFolderItemSelected(item);
  }
}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::btnGotoHome_Click(TObject *Sender)
{
  gotoHome();
}
//---------------------------------------------------------------------------

void __fastcall TFmeBrowseForFolder::edCurFile_ChangeTracking(TObject *Sender)
{
  if( !edCurFile_->ReadOnly )
  {
    if( m_onChange )
      m_onChange(this);
  }
}
//---------------------------------------------------------------------------


