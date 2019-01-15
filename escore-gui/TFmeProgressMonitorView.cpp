#include "escore_gui_pch.h"
#pragma hdrstop

#include "TFmeProgressMonitorView.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
// progress monitor tree view item class. besides standard text, which is used
// as task-text, it contains optional progress text, and progress bar|indetermined progress indicator
//
class EsProgressViewItem : public TTreeViewItem
{
public:
	__fastcall EsProgressViewItem(TComponent* owner);

	void __fastcall onApplyStyleLookup(TObject* Sender);

	void textSet(const EsString& text);
	void pulsingSet(bool pulsing);
	void progressTextSet(const EsString& text);
	void progressSet(const EsVariant& val);

protected:
	TRectangle* m_rTaskText;
	TText* m_taskText;
	TRectangle* m_rProgress;
	TText* m_progressText;
	TAniIndicator* m_pulser;
	TProgressBar* m_progress;
};

__fastcall EsProgressViewItem::EsProgressViewItem(TComponent* owner) :
TTreeViewItem(owner),
m_rTaskText(0),
m_taskText(0),
m_rProgress(0),
m_progressText(0),
m_pulser(0),
m_progress(0)
{
	m_rTaskText = new TRectangle(this);
	ES_ASSERT(m_rTaskText);
	m_rTaskText->Align = TAlignLayout::Top;
	m_rTaskText->Parent = this;
  m_rTaskText->Height = 20;
	m_rTaskText->StyleName = esT("taskTextRect");

	m_rTaskText->Stroke->Color =
#ifdef ES_IS_MOBILE_OS
    TAlphaColor(0);
#else
    TAlphaColorRec::Null;
#endif

	m_rTaskText->Fill->Color =
#ifdef ES_IS_MOBILE_OS
    TAlphaColor(0);
#else
    TAlphaColorRec::Null;
#endif

	m_taskText = new TText(this);
	ES_ASSERT(m_taskText);
	m_taskText->Align = TAlignLayout::Client;
	m_taskText->Parent = m_rTaskText;
	m_taskText->StyleName = esT("taskText");

	m_rProgress = new TRectangle(this);
	ES_ASSERT(m_rProgress);
	m_rProgress->Align = TAlignLayout::Client;
	m_rProgress->Parent = this;
	m_rProgress->StyleName = esT("progressRect");

 	m_rProgress->Stroke->Color =
#ifdef ES_IS_MOBILE_OS
    TAlphaColor(0);
#else
    TAlphaColorRec::Null;
#endif

	m_rProgress->Fill->Color =
#ifdef ES_IS_MOBILE_OS
    TAlphaColor(0);
#else
    TAlphaColorRec::Null;
#endif

	m_pulser = new TAniIndicator(this);
	ES_ASSERT(m_pulser);
	m_pulser->Enabled = false;
	m_pulser->Visible = false;
	m_pulser->Align = TAlignLayout::Client;
	m_pulser->Parent = m_rProgress;
	m_pulser->StyleName = esT("pulser");

	m_progress = new TProgressBar(this);
	ES_ASSERT(m_progress);
	m_progress->Value = 0; m_progress->Min = 0; m_progress->Max = 1;
	m_progress->Align = TAlignLayout::Client;
	m_progress->Parent = m_rProgress;

	m_progressText = new TText(this);
	ES_ASSERT(m_progressText);
	m_progressText->Align = TAlignLayout::Client;
	m_progressText->Parent = m_rProgress;
	m_progressText->StyleName = esT("progressText");

  Height = m_rTaskText->Height*2;

	StyleLookup = esT("progressMonitorItemStyle");
	OnApplyStyleLookup = onApplyStyleLookup;
}

void __fastcall EsProgressViewItem::onApplyStyleLookup(TObject* Sender)
{
  float h = m_rTaskText->Height*2;

	TRectangle* r = dynamic_cast<TRectangle*>( FindStyleResource(m_rTaskText->StyleName) );
	if( r )
	{
		m_rTaskText->Padding->Assign( r->Padding );
		m_rTaskText->Margins->Assign( r->Margins );
		m_rTaskText->Height = r->Height;
    h = r->Height;
		m_rTaskText->Stroke->Assign( r->Stroke );
		m_rTaskText->Fill->Assign( r->Fill );
	}

	TText* txt = dynamic_cast<TText*>( FindStyleResource(m_taskText->StyleName) );
	if( txt )
	{
		m_taskText->HorzTextAlign = txt->HorzTextAlign;
		m_taskText->VertTextAlign = txt->VertTextAlign;
		m_taskText->Font->Assign( txt->Font );
		m_taskText->Color = txt->Color;
	}

	r = dynamic_cast<TRectangle*>( FindStyleResource(m_rProgress->StyleName) );
	if( r )
	{
		m_rProgress->Padding->Assign( r->Padding );
		m_rProgress->Margins->Assign( r->Margins );
		h += r->Height;
		m_rProgress->Stroke->Assign( r->Stroke );
		m_rProgress->Fill->Assign( r->Fill );
	}

	txt = dynamic_cast<TText*>( FindStyleResource(m_progressText->StyleName) );
	if( txt )
	{
		m_progressText->HorzTextAlign = txt->HorzTextAlign;
		m_progressText->VertTextAlign = txt->VertTextAlign;
		m_progressText->Font->Assign( txt->Font );
		m_progressText->Color = txt->Color;
	}

  Height = h;
}

void EsProgressViewItem::textSet(const EsString& text)
{
	m_taskText->Text = text.c_str();
}

void EsProgressViewItem::pulsingSet(bool pulsing)
{
  EsUpdateLocker<EsProgressViewItem> lock(this);
  if(pulsing)
  {
    m_progress->Visible = false;
    m_pulser->Visible = true;
    m_pulser->Enabled = true;
  }
  else
  {
    m_pulser->Enabled = false;
    m_pulser->Visible = false;
    m_progress->Visible = true;
  }
}

void EsProgressViewItem::progressTextSet(const EsString& text)
{
	m_progressText->Text = text.c_str();
}

void EsProgressViewItem::progressSet(const EsVariant& val)
{
	if( val.isEmpty() )
		pulsingSet(true);
	else
	{
		m_progress->Value = val.asDouble();
		if( !m_progress->Visible )
			pulsingSet(false);
	}
}

//---------------------------------------------------------------------------
// EsEvent handler implementation for progress view
TFmeProgressMonitorView::Handler::Handler(TFmeProgressMonitorView* This) :
EsEventSubscriber(EsString::null()),
m_this(This)
{
	ES_ASSERT(m_this);
	activeSet(false);
}

void TFmeProgressMonitorView::Handler::notify(const EsEventIntf::Ptr& evt)
{
	ES_ASSERT(evt);
  const EsString& cat = evt->categoryGet();
  if( 0 == cat.find(EVTC_PROGRESS_MONITOR_PFX) )
  {
    ulong id = evt->idGet();
    const EsVariant& payload = evt->payloadGet();
    switch(id)
    {
    case evtTaskTextChanged:
      m_this->onTaskTextChanged(payload);
      break;
    case evtTaskStateChanged:
      m_this->onTaskStateChanged(payload);
      break;
    case evtTaskProgressChanged:
      m_this->onTaskProgressChanged(payload);
      break;
    case evtTaskAdded:
      m_this->onTaskAdded(payload);
      break;
    case evtTaskRemoved:
      m_this->onTaskRemoved(payload);
      break;
    case evtNoMoreTasks:
      m_this->viewClear();
      break;
    }
  }
}

//---------------------------------------------------------------------------
// progress view implementation
//
__fastcall TFmeProgressMonitorView::TFmeProgressMonitorView(TComponent* Owner) :
TFrame(Owner),
m_handler(this),
m_defHeight(20),
m_items(
  EsStringIndexedMap::ContainerUsesInterlock,
  esT("ProgressMonitorView")
)
{
  txtReady_->Text = _("Ready");
  saveDefaultHeight();
}
//---------------------------------------------------------------------------

__fastcall TFmeProgressMonitorView::~TFmeProgressMonitorView()
{}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::saveDefaultHeight()
{
  m_defHeight = Height;
  m_realHeight = m_defHeight;
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::progressMonitorSet(const EsProgressMonitorIntf::Ptr& mon)
{
	m_mon = mon;
	if( m_mon )
	{
		m_handler.categoriesSet(m_mon->eventCategoryGet());
		m_handler.activeSet(true);
		// try to build view of existing progress monitor contents
		viewRebuild();
	}
	else
	{
		// deactivate handler
		m_handler.activeSet(false);
		// clear view contents
		viewClear();
	}
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::viewClear()
{
	tvTasks_->Clear();
	m_items.clear();
	tvTasks_->Visible = false;
	txtReady_->Visible = true;

  heightUpdate();

	if( m_onReady )
		m_onReady(this);
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::viewRebuild()
{
  {
    EsUpdateLocker<TTreeView> lock(tvTasks_);
    viewClear();
    ES_ASSERT(m_mon);
    const EsStringArray& taskPaths = m_mon->taskPathsGet();
    if( !taskPaths.empty() )
    {
      txtReady_->Visible = false;
      tvTasks_->Visible = true;
      for(size_t idx = 0; idx < taskPaths.size(); ++idx)
      {
        const EsString& path = taskPaths[idx];
        EsProgressMonitorTaskIntf::Ptr task = m_mon->taskGetByPath(path);
        EsProgressViewItem* taskView = dynamic_cast<EsProgressViewItem*>(taskViewCreate(task));
      }
    }
  }

  heightUpdate();
  tvTasks_->RealignContent();
}
//---------------------------------------------------------------------------

TTreeViewItem* TFmeProgressMonitorView::taskItemFind(const EsString& path, size_t& idx) const
{
	TTreeViewItem* item = 0;
	idx = m_items.itemFind(path);

	if( EsStringIndexedMap::npos != idx )
	{
		item = reinterpret_cast<TTreeViewItem*>(m_items.valueGet(idx).asPointer());
		ES_ASSERT(item);
	}

	return item;
}
//---------------------------------------------------------------------------

TTreeViewItem* TFmeProgressMonitorView::parentTaskItemFind(const EsString& path) const
{
	TTreeViewItem* item = 0;
	EsPath p = EsPath::createFromPath(path);
	if( p.isOk() )
	{
		const EsStringArray& dirs = p.dirsGet();
		if( 1 < dirs.size() ) // is not top task path
		{
			p.dirLastRemove(); // contruct parent path
			const EsString& parentPath = p.pathGet(static_cast<ulong>(EsPathFlag::AsIs));
			size_t idx;
			item = taskItemFind(parentPath, idx);

			if( EsStringIndexedMap::npos == idx )
				EsException::Throw(
          esT("Could not find parent task with path '%s'"),
          parentPath
        );
		}
	}
	else
		EsException::Throw(
      esT("Could not parse task path: '%s'"),
      path
    );

	return item;
}
//---------------------------------------------------------------------------

TTreeViewItem* TFmeProgressMonitorView::taskViewCreate(const EsProgressMonitorTaskIntf::Ptr& task)
{
	return taskViewCreate(task->pathGet(), task->textGet(), task->progressTextGet(),
		task->completionGet());
}
//---------------------------------------------------------------------------

TTreeViewItem* TFmeProgressMonitorView::taskViewCreate(const EsString& path,
	const EsString& text, const EsString& progressText, const EsVariant& completion )
{
	EsProgressViewItem* newItem = 0;
	size_t idx;
	taskItemFind(path, idx);
	if( EsStringIndexedMap::npos == idx )
	{
		TTreeViewItem* parentItem = parentTaskItemFind(path);
		TFmxObject* owner;
		if( parentItem )
    	owner = parentItem;
		else // this is top-level task item
			owner = tvTasks_;

		newItem = new EsProgressViewItem(owner);
		ES_ASSERT(newItem);
		newItem->textSet(text);
		newItem->progressTextSet(progressText);
		newItem->progressSet(completion);

    tvTasks_->ItemHeight = newItem->Height;

		owner->AddObject(newItem);

		m_items.itemAdd(path, EsVariant(newItem, EsVariant::ACCEPT_POINTER));

		if( !tvTasks_->Visible )
		{
			txtReady_->Visible = false;
			tvTasks_->Visible = true;
		}

    tvTasks_->ExpandAll();
    heightUpdate();
	}
	else
		EsException::Throw(
      esT("Task view item with path '%s' already exists in '%s'"),
			path,
      m_items.nameGet()
    );

	return newItem;
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::taskViewDestroy(const EsString& path)
{
	size_t idx;
	TTreeViewItem* item = taskItemFind(path, idx);
	if(EsStringIndexedMap::npos != idx)
	{
		ES_ASSERT(item);
		delete item;

		m_items.itemDelete(idx);

    heightUpdate();
	}
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::heightUpdate()
{
  float borderH = 2;
  float newHeight = 0;
  for( size_t idx = 0; idx < m_items.countGet(); ++idx )
  {
    TTreeViewItem* item =
      reinterpret_cast<TTreeViewItem*>(m_items.valueGet(idx).asPointer());
    ES_ASSERT(item);

    newHeight += item->Height + 2*borderH;
  }
  newHeight = esMax( m_defHeight, newHeight );
  m_realHeight = newHeight;
  Height = newHeight;
}
//---------------------------------------------------------------------------

// EsEvent handlers
void TFmeProgressMonitorView::onTaskTextChanged(const EsVariant& payload)
{
	const EsString& path = payload.itemGet(0).asString();
	const EsString& newText = payload.itemGet(2).asString();
	size_t idx;
	EsProgressViewItem* item =
		dynamic_cast<EsProgressViewItem*>(taskItemFind(path, idx));
	ES_ASSERT(item);

	item->textSet(newText);

	if( m_onActivity )
		m_onActivity( this );
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::onTaskStateChanged(const EsVariant& payload)
{
	const EsString& path = payload.itemGet(0).asString();
	const EsString& progressText = payload.itemGet(2).asString();
	const EsVariant& completion = payload.itemGet(3);

	size_t idx;
	EsProgressViewItem* item =
		dynamic_cast<EsProgressViewItem*>(taskItemFind(path, idx));
	ES_ASSERT(item);

	item->progressTextSet(progressText);
	item->progressSet(completion);

	if( m_onActivity )
		m_onActivity( this );
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::onTaskProgressChanged(const EsVariant& payload)
{
	const EsString& path = payload.itemGet(0).asString();
	const EsString& progressText = payload.itemGet(1).asString();
	const EsVariant& completion = payload.itemGet(2);

	size_t idx;
	EsProgressViewItem* item =
		dynamic_cast<EsProgressViewItem*>(taskItemFind(path, idx));
	ES_ASSERT(item);

	item->progressTextSet(progressText);
	item->progressSet(completion);

	if( m_onActivity )
		m_onActivity( this );
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::onTaskAdded(const EsVariant& payload)
{
	const EsString& path = payload.itemGet(0).asString();
	const EsString& text = payload.itemGet(1).asString();
	const EsString& progressText = payload.itemGet(3).asString();
	const EsVariant& completion = payload.itemGet(4);

	taskViewCreate(path, text, progressText, completion);
  tvTasks_->RealignContent();

	if( m_onActivity )
		m_onActivity( this );
}
//---------------------------------------------------------------------------

void TFmeProgressMonitorView::onTaskRemoved(const EsVariant& payload)
{
	taskViewDestroy(payload.asString());

	if( m_onActivity )
		m_onActivity( this );
}
//---------------------------------------------------------------------------

