#include "escore_gui_pch.h"
#pragma hdrstop

#include "TDlgBrowseForFolder.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "TDlgBase"
#pragma link "TFmeBrowseForFolder"
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
__fastcall TDlgBrowseForFolder::TDlgBrowseForFolder(TComponent* Owner) :
TDlgBase(Owner)
{
  browser_->onChange = onChange;
#ifdef ES_IS_MOBILE_OS
  layBtns_->Visible = false;
  layBtns_->Parent = 0;

  btnOk_->Align = TAlignLayout::MostRight;
  btnOk_->Width = 50;
  if( browser_->showFilesGet() )
    btnOk_->Parent = browser_->layCurFile_;
  else
    btnOk_->Parent = browser_->layCurFolder_;
#endif
  btnOkUpdate();
}
//---------------------------------------------------------------------------

__fastcall TDlgBrowseForFolder::~TDlgBrowseForFolder()
{}
//---------------------------------------------------------------------------

void __fastcall TDlgBrowseForFolder::imagesSet(TCustomImageList* images)
{
  browser_->images = images;
}
//---------------------------------------------------------------------------

/// Return true if file item is selected in list
bool TDlgBrowseForFolder::isFileSelected() const
{
  return browser_->isFileSelected();
}
//---------------------------------------------------------------------------

/// Return true if folder item is selected in list
bool TDlgBrowseForFolder::isFolderSelected() const
{
  return browser_->isFolderSelected();
}
//---------------------------------------------------------------------------

/// Return true if home view is showing
bool TDlgBrowseForFolder::isHome() const
{
  return browser_->isHome();
}
//---------------------------------------------------------------------------

/// Access currently selected (and listed) folder. If current folder is set to
/// some destination outside the rootFolder, or an empty string, its value got
/// re-set to the rootFolder value.
///
const EsString& TDlgBrowseForFolder::currentFolderGet() const
{
	return browser_->currentFolderGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::currentFolderSet(const EsString& folder)
{
	browser_->currentFolderSet(folder);
  btnOkUpdate();
}
//---------------------------------------------------------------------------

/// Root folder property specifies top most folder, which may be browsed.
/// Setting root folder property re-sets current folder property to the value of the root folder
///
const EsStringArray& TDlgBrowseForFolder::rootFoldersGet() const
{
	return browser_->rootFoldersGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::rootFoldersSet(const EsStringArray& rootFolders)
{
	browser_->rootFoldersSet(rootFolders);
}
//---------------------------------------------------------------------------

/// Show files as well
bool TDlgBrowseForFolder::showFilesGet() const
{
  return browser_->showFilesGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::showFilesSet( bool show )
{
  browser_->showFilesSet(show);
}
//---------------------------------------------------------------------------

bool TDlgBrowseForFolder::flagFileNameEditableGet() const
{
  return browser_->flagFileNameEditableGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::flagFileNameEditableSet( bool editable )
{
  browser_->flagFileNameEditableSet( editable );
}
//---------------------------------------------------------------------------

/// Access file mask, otherwise, all files in currently selected folder are shown
const EsString& TDlgBrowseForFolder::filePatternGet() const
{
  return browser_->filePatternGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::filePatternSet(const EsString& patt)
{
  browser_->filePatternSet(patt);
}
//---------------------------------------------------------------------------

/// Access currently selected file
EsString TDlgBrowseForFolder::currentFileGet() const
{
  return browser_->currentFileGet();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::currentFileSet(const EsString& file)
{
  browser_->currentFileSet(file);
  btnOkUpdate();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::btnOkUpdate()
{
  if( browser_->showFilesGet() )
    btnOk_->Enabled = browser_->isFileSelected();
  else
    btnOk_->Enabled = !currentFolderGet().empty();//browser_->isFolderSelected();
}
//---------------------------------------------------------------------------

void __fastcall TDlgBrowseForFolder::onChange(TObject* sender)
{
  btnOkUpdate();
}
//---------------------------------------------------------------------------

void TDlgBrowseForFolder::gotoHome()
{
  browser_->gotoHome();
}
//---------------------------------------------------------------------------

void __fastcall TDlgBrowseForFolder::BeginUpdate()
{
  TDlgBase::BeginUpdate();
  browser_->BeginUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TDlgBrowseForFolder::EndUpdate()
{
  TDlgBase::EndUpdate();
  browser_->EndUpdate();
}
//---------------------------------------------------------------------------


