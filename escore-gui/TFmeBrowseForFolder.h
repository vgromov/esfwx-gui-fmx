#ifndef TFmeBrowseForFolderH
#define TFmeBrowseForFolderH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
//---------------------------------------------------------------------------

/// Simple folder browser frame. Currently selected folder is
/// accessible via currentFolder property. Root folder property specifies
/// top most folder, which may be browsed.
///
class ESCORE_GUI_PKG TFmeBrowseForFolder : public TFrame
{
__published:	// IDE-managed Components
	TListBox *lbFolders_;
	TEdit *edCurFolder_;
  TLayout *layCurFolder_;
  TSpeedButton *btnGotoHome_;
  TLayout *layCurFile_;
  TEdit *edCurFile_;
  TLabel *lblCurFolder_;
  TLabel *lblCurFile_;
	void __fastcall lbFolders_ItemClick(const TCustomListBox *Sender, const TListBoxItem *Item);
  void __fastcall lbFolders_DblClick(TObject *Sender);
  void __fastcall btnGotoHome_Click(TObject *Sender);
  void __fastcall edCurFile_ChangeTracking(TObject *Sender);

protected:
	enum {
		evtFolderListingUpdated,
    evtFilesListingUpdated
	};

	class Subscriber : public EsEventSubscriber
	{
	public:
		Subscriber(TFmeBrowseForFolder& owner);

		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		TFmeBrowseForFolder& m_owner;
	};
	friend class Subscriber;

public:
  /// Functionality flags
  enum {
    flagShowFiles       = 0x00000001,
    flagFileNameEditable= 0x00000002,
    flagListingPending  = 0x00010000,
    flagHomeView        = 0x00020000,
   };

public:		// User declarations
	__fastcall TFmeBrowseForFolder(TComponent* Owner);
  virtual __fastcall ~TFmeBrowseForFolder();

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

	/// Root folders property specifies top most folders, which may be browsed.
	/// Setting root folders property re-sets current folder to the empty value,
  /// as if gotoHome call was made
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
  virtual void __fastcall EndUpdate();

  /// Selection change handler
  __property TNotifyEvent onChange = {read=m_onChange, write=m_onChange};

  /// Set-up image list for file browser && buttons
  __property TCustomImageList* images = {write=imagesSet};

private:
  bool currentIsOneOfRootFolders() const;
	void currentFolderList();
	void listingSet(const EsString::Array& folders, const EsString::Array& files);
  const EsString& findRootForFolder(const EsString& folder) const;
  void onFolderItemSelected(const TListBoxItem *item);

  void __fastcall imagesSet(TCustomImageList* images);

private:
	Subscriber m_evtSub;
	EsStringArray m_rootFolders;
  EsString m_currentRoot;
	EsString m_currentFolder;
  ulong m_flags;
  int m_idxImgFolderUp;
  int m_idxImgFolder;
  int m_idxImgFile;
  EsString m_filePatt;
  EsString m_currentFile;
  TNotifyEvent m_onChange;
};

//---------------------------------------------------------------------------
#endif
