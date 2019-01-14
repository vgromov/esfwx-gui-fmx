#ifndef TFmeAppLogH
#define TFmeAppLogH
//---------------------------------------------------------------------------
#include <System.Rtti.hpp>
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Grid.hpp>
#include <FMX.ActnList.hpp>
#include <FMX.Menus.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Grid.Style.hpp>
#include <FMX.ScrollBox.hpp>
#if 0x0720 <= __CODEGEARC__
# include <FMX.Grid.Style.hpp>
#endif
#include <FMX.ScrollBox.hpp>
//---------------------------------------------------------------------------

class ESCORE_GUI_PKG TFmeAppLog : public TFrame
{
__published:	// IDE-managed Components
	TGrid *logView_;
  TStringColumn *colMessage_;
	TStringColumn *colTimestamp_;
	TStringColumn *colLevel_;
	TStringColumn *colData_;
  TPopupMenu *mnu_;
  TActionList *acts_;
  TAction *actFile_;
  TAction *actFileSave_;
  TAction *actActions_;
  TAction *actActionsClear_;
  TMenuItem *mnuFile_;
  TMenuItem *mnuActions_;
  TMenuItem *mnuFileSave_;
  TMenuItem *mnuActionsClear_;
  TAction *actConfigDumpFolder_;
  TAction *actConfig_;
  TAction *actConfigDoDump_;
  TMenuItem *mnuConfig_;
  TMenuItem *mnuConfigDumpFolder_;
  TMenuItem *mnuConfigDoDump_;
	void __fastcall logView_GetValue(TObject *Sender, const int Col, const int Row,
          TValue &Value);
  void __fastcall actDummy_Execute(TObject *Sender);
  void __fastcall actActionsClear_Update(TObject *Sender);
  void __fastcall actActionsClear_Execute(TObject *Sender);
  void __fastcall actFileSave_Execute(TObject *Sender);
  void __fastcall actFileSave_Update(TObject *Sender);
  void __fastcall actConfigDumpFolder_Execute(TObject *Sender);
  void __fastcall actConfigDoDump_Update(TObject *Sender);
  void __fastcall actConfigDoDump_Execute(TObject *Sender);

private:
	class Handler : public EsEventSubscriber
	{
	public:
		Handler(TFmeAppLog& This);

		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	private:
		TFmeAppLog& m_this;
	};
	friend class Handler;

	class LogItem
	{
	public:
		LogItem() {}
		/// NB! val must be empty or convertible to string
		LogItem(const EsDateTime& ts,	ulong level, const EsString& msg,	const EsVariant& val);

	public:
		EsDateTime m_ts;
		ulong m_level;
		EsString m_msg;
		EsString m_val;
	};
	typedef std::map<esDT, LogItem> LogItems;

public:
	__fastcall TFmeAppLog(TComponent* Owner);
  virtual __fastcall ~TFmeAppLog();

  virtual void i18nInit();

	bool activeGet() const;
	void activeSet(bool active);

	bool empty() const;
	void clear();

	ulong logDepthGet() const;
	void logDepthSet(ulong depth);

  llong logDumpSizeGet() const;
  void logDumpSizeSet(llong size);

	const EsString& dumpFolderGet() const;
	void dumpFolderSet(const EsString& folder);

	bool performFileDumpGet() const;
	void performFileDumpSet(bool doDump);

	void settingsLoad(const EsStreamIntf::Ptr& stream);
	void settingsSave(const EsStreamIntf::Ptr& stream);

protected:
	void onLogEvent(const EsDateTime& ts,	ulong level, const EsString& msg,	const EsVariant& val);
	void logViewUpdate();
	void logDepthRestrict();
  void dumpToFile() const;
  void columnWidthsLoad(const EsStreamIntf::Ptr& stream);
  void columnWidthsSave(const EsStreamIntf::Ptr& stream);

  static EsString dumpFileNameConstruct(const EsString& folder);
  static void dumpEntryAppend(EsFile& f, const TFmeAppLog::LogItem& li,
    const EsString& folder, bool shouldOpenFile,
    bool restrictFileSize, llong sizeRestriction);
	static EsString levelStringGet(ulong level);

  void __fastcall onFolderPickClose(TDlgBase* sender, TModalResult mr, const EsVariant& payload);
  // Overridable I18N handler
  void __fastcall onI18nStringsUpdate(TObject* sender);

private:
  EsI18nManagerScope m_i18nScope;
	Handler m_handler;
	LogItems m_items;
	ulong m_depth;
  llong m_dumpSize;
	EsString m_dumpFolder;
  EsFile m_dump;
	bool m_doFileDump;
};

//---------------------------------------------------------------------------
#endif
