#include "escore_gui_pch.h"
#pragma hdrstop

#include "TFmeAppLog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
enum {
  logDepthMin         = 100,
  logDepthDef         = 1000,
  logDepthMax         = 10000,

  logDumpFileSizeMin  = 1024,
  logDumpFileSizeDef  = 2048,
  logDumpFileSizeMax  = 10000
};

//---------------------------------------------------------------------------
TFmeAppLog::Handler::Handler(TFmeAppLog& This) :
EsEventSubscriber(esT("applog")),
m_this(This)
{}
//---------------------------------------------------------------------------

// event notification interface
void TFmeAppLog::Handler::notify(const EsEventIntf::Ptr& evt)
{
	ES_ASSERT(evt);
	const EsString& cat = evt->categoryGet();
	ES_ASSERT(esT("applog") == cat);
	const EsDateTime& ts = evt->timestampGet();
	ulong level = evt->idGet();
	const EsVariant::Array& payload = evt->payloadGet().asVariantCollection();
	const EsString& msg = payload[0].asString();
	const EsVariant& val = payload[1];

	m_this.onLogEvent(ts,	level, msg,	val);
}
//---------------------------------------------------------------------------

TFmeAppLog::LogItem::
LogItem(const EsDateTime& ts,	ulong level, const EsString& msg,	const EsVariant& val) :
m_ts(ts),
m_level(level),
m_msg(msg)
{
	if( !val.isEmpty() )
	{
		try
		{
    	m_val = val.asString();
		}
		catch(...)
		{}
	}
}

//---------------------------------------------------------------------------
__fastcall TFmeAppLog::TFmeAppLog(TComponent* Owner) :
TFrame(Owner),
m_i18nScope(this),
m_handler(*this),
m_depth(logDepthDef),
m_dumpSize(logDumpFileSizeDef),
m_doFileDump(false)
{
  onI18nStringsUpdate(nullptr);
}
//---------------------------------------------------------------------------

__fastcall TFmeAppLog::~TFmeAppLog()
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::~TFmeAppLog"));

  m_i18nScope.managerSet(nullptr);
  EsUtilities::actionClientUnregisterFix(this);
}
//---------------------------------------------------------------------------

void TFmeAppLog::i18nInit()
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::i18nInit"));

  m_i18nScope.onGuiStringsUpdate = onI18nStringsUpdate;
	m_i18nScope.managerSet(EsI18nManager::instanceGet());
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::onI18nStringsUpdate(TObject* sender)
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::onI18nStringsUpdate"));

  colTimestamp_->Header = _("Timestamp");
  colLevel_->Header = _("Level");
  colMessage_->Header = _("Message");
  colData_->Header = _("Data");
}
//---------------------------------------------------------------------------

bool TFmeAppLog::activeGet() const
{
	return m_handler.activeGet();
}
//---------------------------------------------------------------------------

void TFmeAppLog::activeSet(bool active)
{
  if( !active && m_dump.get_opened() )
    m_dump.close();

	m_handler.activeSet(active);
}
//---------------------------------------------------------------------------

bool TFmeAppLog::empty() const
{
	return m_items.empty();
}
//---------------------------------------------------------------------------

void TFmeAppLog::clear()
{
	m_items.clear();
	logViewUpdate();
}
//---------------------------------------------------------------------------

ulong TFmeAppLog::logDepthGet() const
{
	return m_depth;
}
//---------------------------------------------------------------------------

void TFmeAppLog::logDepthSet(ulong depth)
{
	if( depth < logDepthMin )
		depth = logDepthMin;
	else if( depth > logDepthMax )
		depth = logDepthMax;

	if( m_depth != depth )
	{
		m_depth = depth;
		logDepthRestrict();
	}
}
//---------------------------------------------------------------------------

llong TFmeAppLog::logDumpSizeGet() const
{
  return m_dumpSize;
}
//---------------------------------------------------------------------------

void TFmeAppLog::logDumpSizeSet(llong size)
{
  if( size < logDumpFileSizeMin )
    size = logDumpFileSizeMin;
  else if( size > logDumpFileSizeMax )
    size = logDumpFileSizeMax;

  if( m_dumpSize != size )
  {
    m_dumpSize = size;
    if( m_dump.get_opened() &&
        m_dump.get_length() > m_dumpSize )
      m_dump.close();
  }
}
//---------------------------------------------------------------------------

const EsString& TFmeAppLog::dumpFolderGet() const
{
	return m_dumpFolder;
}
//---------------------------------------------------------------------------

void TFmeAppLog::dumpFolderSet(const EsString& folder)
{
	if( m_dumpFolder != folder )
	{
		if( !EsPath::dirExists(folder, EsString::null()) )
			EsException::Throw(
        esT("Log dump folder '%s' does not exist"),
        folder
      );

    m_dumpFolder = folder;

    if( m_dump.get_opened() )
      m_dump.close();
	}
}
//---------------------------------------------------------------------------

bool TFmeAppLog::performFileDumpGet() const
{
	return m_doFileDump;
}
//---------------------------------------------------------------------------

void TFmeAppLog::performFileDumpSet(bool doDump)
{
	m_doFileDump = doDump;

  if( !m_doFileDump && m_dump.get_opened() )
    m_dump.close();
}
//---------------------------------------------------------------------------

void TFmeAppLog::columnWidthsLoad(const EsStreamIntf::Ptr& stream)
{
  ES_ASSERT(stream);
  for(int idx = 0; idx < logView_->ColumnCount; ++idx)
  {
    TColumn* col = logView_->Columns[idx];
    float w = stream->valueRead(
      EsString::format(esT("logViewCol%d"), idx).c_str(),
      100.
    ).asDouble();

    if( w > 2000 )
      w = 2000;
    if( w < 100 )
      w = 100;

    col->Width = w;
  }
}
//---------------------------------------------------------------------------

void TFmeAppLog::columnWidthsSave(const EsStreamIntf::Ptr& stream)
{
  ES_ASSERT(stream);
  for(int idx = 0; idx < logView_->ColumnCount; ++idx)
  {
    TColumn* col = logView_->Columns[idx];
    stream->valueWrite(
      EsString::format(esT("logViewCol%d"), idx).c_str(),
      col->Width
    );
  }
}
//---------------------------------------------------------------------------

void TFmeAppLog::settingsLoad(const EsStreamIntf::Ptr& stream)
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::settingsLoad"));

	EsStreamContextExistingScope scope(stream, esT("AppLog"));
	if( scope.isOk() )
	{
		logDepthSet( stream->valueRead(esT("Depth"), logDepthMax).asULong() );
		dumpFolderSet( stream->valueRead(esT("DumpFolder"), EsString::null()).asString() );
		performFileDumpSet( stream->valueRead(esT("DoDump"), false).asBool() );
    activeSet( stream->valueRead(esT("Active"), true).asULong() );
    columnWidthsLoad( stream );
	}
}
//---------------------------------------------------------------------------

void TFmeAppLog::settingsSave(const EsStreamIntf::Ptr& stream)
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::settingsSave"));

	EsStreamContextExistingOrNewScope scope(stream, esT("AppLog"));
	if( scope.isOk() )
	{
		stream->valueWrite(esT("Active"), activeGet());
		stream->valueWrite(esT("Depth"), m_depth);
		stream->valueWrite(esT("DumpFolder"), m_dumpFolder);
		stream->valueWrite(esT("DoDump"), m_doFileDump);
    columnWidthsSave( stream );
	}
}
//---------------------------------------------------------------------------

void TFmeAppLog::logViewUpdate()
{
	EsUpdateLocker<TGrid> lock(logView_);
	logView_->RowCount = (int)m_items.size();
}
//---------------------------------------------------------------------------

void TFmeAppLog::logDepthRestrict()
{
	// remove oldest item(s) from log
	while( m_items.size() > m_depth )
		m_items.erase( m_items.begin() );
	logViewUpdate();
}
//---------------------------------------------------------------------------

void TFmeAppLog::onLogEvent(const EsDateTime& ts,	ulong level, const EsString& msg,	const EsVariant& val)
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::onLogEvent"));

  const LogItem& li = LogItem(ts, level, msg, val);
	m_items[(esDT)ts] = li;
  dumpEntryAppend(m_dump, li, m_dumpFolder, m_doFileDump, true, m_dumpSize);
	logDepthRestrict();
	if( logView_->RowCount )
		logView_->Selected = logView_->RowCount-1;
}
//---------------------------------------------------------------------------

void TFmeAppLog::dumpToFile() const
{
  ES_DEBUG_TRACE(esT("TFmeAppLog::dumpToFile"));

  UnicodeString dir = m_dumpFolder.c_str();
  if( SelectDirectory(_("Select folder to dump log file to"), EmptyStr, dir) )
  {
    const EsString& dfn = dumpFileNameConstruct(dir.c_str());
    EsFile df(
      dfn,
      static_cast<ulong>(EsFileFlag::Write)|
      static_cast<ulong>(EsFileFlag::Append)|
      static_cast<ulong>(EsFileFlag::Exclusive)|
      static_cast<ulong>(EsFileFlag::Text)
    );

    if( df.open() )
    {
      for(LogItems::const_iterator cit = m_items.begin(); cit != m_items.end(); ++cit)
      {
        const LogItem& li = (*cit).second;
        dumpEntryAppend(df, li, dir.c_str(), true, false, 0);
      }

      df.close();
    }
  }
}
//---------------------------------------------------------------------------

EsString TFmeAppLog::dumpFileNameConstruct(const EsString& folder)
{
  EsPath dp = EsPath::createFromPath(folder);
  EsString logName = ExtractFileName(ParamStr(0)).c_str();

  logName += esT(" ") + EsDateTime::now().asString();
  logName = EsPath::makeSafe(logName);
  dp.fileNameSet(logName);
  dp.fileExtSet(esT("log"));

  return dp.pathGet();
}
//---------------------------------------------------------------------------

void TFmeAppLog::dumpEntryAppend(EsFile& f, const TFmeAppLog::LogItem& li,
  const EsString& folder, bool shouldOpenFile, bool restrictFileSize,
  llong sizeRestriction)
{
  if( !f.get_opened() &&
      shouldOpenFile )
  {
    const EsString& df = dumpFileNameConstruct(folder);

    f.set_name( df );
    f.set_flags(
      static_cast<ulong>(EsFileFlag::Write)|
      static_cast<ulong>(EsFileFlag::Append)|
      static_cast<ulong>(EsFileFlag::Exclusive)|
      static_cast<ulong>(EsFileFlag::Text)
    );
    f.open();
  }

  if( f.get_opened() )
  {
    const EsDateTime& dt = li.m_ts;
    const EsString& line =
      EsString::format(
        esT("%s.%0.3lu %s %s %s\n"),
        dt.asString(),
        dt.get_milliseconds(),
        levelStringGet(li.m_level),
        li.m_msg,
        li.m_val
      );
    f.stringWrite(line);

    if( restrictFileSize &&
        f.get_length() > sizeRestriction )
      f.close();
  }
}
//---------------------------------------------------------------------------

EsString TFmeAppLog::levelStringGet(ulong level)
{
	switch( static_cast<EsAppLogLevel>(level) )
	{
	case EsAppLogLevel::Info:
		return _("Information");
	case EsAppLogLevel::Warning:
		return _("Warning");
	case EsAppLogLevel::Error:
		return _("Error");
	case EsAppLogLevel::Debug:
		return _("Debug");
	default:
		return _("Custom");
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::onFolderPickClose(TDlgBase* sender, TModalResult mr, const EsVariant& ES_UNUSED(payload))
{
  if( IsPositiveResult(mr) )
  {
    TDlgBrowseForFolder* dlg = dynamic_cast<TDlgBrowseForFolder*>(sender);
    ES_ASSERT(dlg);

    m_dumpFolder = dlg->currentFolderGet();
  }
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::logView_GetValue(TObject *Sender, const int Col, const int Row,
					TValue &Value)
{
	ES_ASSERT(Row < (int)m_items.size());
	LogItems::const_iterator cit = m_items.begin();
	std::advance(cit, Row);
	const LogItem& item = (*cit).second;
	switch( Col )
	{
	case 0:
		Value = TValue::_op_Implicit(
			UnicodeString(item.m_ts.asString().c_str()) );
		break;
	case 1:
		Value = TValue::_op_Implicit(
			UnicodeString(levelStringGet(item.m_level).c_str()) );
		break;
	case 2:
		Value = TValue::_op_Implicit(
			UnicodeString(item.m_msg.c_str()) );
		break;
	case 3:
		Value = TValue::_op_Implicit(
			UnicodeString(item.m_val.c_str()) );
		break;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actDummy_Execute(TObject *Sender)
{
  // Dummy top-level actions execution
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actActionsClear_Update(TObject *Sender)
{
  actActionsClear_->Enabled = !empty();
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actActionsClear_Execute(TObject *Sender)
{
  clear();
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actFileSave_Update(TObject *Sender)
{
  actFileSave_->Enabled = !empty();
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actFileSave_Execute(TObject *Sender)
{
  dumpToFile();
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actConfigDumpFolder_Execute(TObject *Sender)
{
  EsUtilities::folderPick(
    this,
    _("Select log dump folder"),
    m_dumpFolder,
    onFolderPickClose,
    dynamic_cast<TCustomImageList*>(acts_->Images)
  );
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actConfigDoDump_Update(TObject *Sender)
{
  actConfigDoDump_->Checked = m_doFileDump;
}
//---------------------------------------------------------------------------

void __fastcall TFmeAppLog::actConfigDoDump_Execute(TObject *Sender)
{
  performFileDumpSet(
    !performFileDumpGet() );
}
//---------------------------------------------------------------------------

