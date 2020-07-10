#ifndef TDlgBrowseForFolderH
#define TDlgBrowseForFolderH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include "TDlgBase.h"
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
#include "TFmeBrowseForFolder.h"
//---------------------------------------------------------------------------

class DELPHICLASS TDlgBrowseForFolder;
class ESCORE_GUI_PKG TDlgBrowseForFolder : public TDlgBase
{
__published:	// IDE-managed Components
	TFmeBrowseForFolder *browser_;

public:		// User declarations
	__fastcall TDlgBrowseForFolder(TComponent* Owner);
  virtual __fastcall ~TDlgBrowseForFolder();

	/// Browser frame delegates
	///
  /// Return true if file item is selected in list
  bool isFileSelected() const;

  /// Return true if folder item is selected in list
  bool isFolderSelected() const;

  /// Return true if home view is showing
  bool isHome() const;

	/// Access currently selected (and listed) folder. If current folder is set to
	/// some destination outside the rootFolder, or an empty string, its value got
	/// re-set to the rootFolder value.
	///
	const EsString& currentFolderGet() const;
	void currentFolderSet(const EsString& folder);

	/// Root folder property specifies top most folder, which may be browsed.
	/// Setting root folder property re-sets current folder property to the value of the root folder
	///
	const EsStringArray& rootFoldersGet() const;
	void rootFoldersSet(const EsStringArray& rootFolders);

  /// Show files as well
  bool showFilesGet() const;
  void showFilesSet( bool show );

  /// Allow to edit file name
  bool flagFileNameEditableGet() const;
  void flagFileNameEditableSet( bool editable );

  /// Access file mask, otherwise, all files in currently selected folder are shown
  const EsString& filePatternGet() const;
  void filePatternSet(const EsString& patt);

  /// Access currently selected file
  EsString currentFileGet() const;
  void currentFileSet(const EsString& file);

  /// Go to home view (to the list of root folders, current folder got reset)
  void gotoHome();

  /// Update locking override.
  /// Useful when changing many properties, which result in file re-listing, at once.
  ///
  virtual void __fastcall BeginUpdate();
  virtual void __fastcall EndUpdate();

  /// Set-up image list for file browser && buttons
  __property TCustomImageList* images = {write=imagesSet};

protected:
  void btnOkUpdate();
  void __fastcall onChange(TObject* sender);

  void __fastcall imagesSet(TCustomImageList* images);
};
//---------------------------------------------------------------------------
#endif
