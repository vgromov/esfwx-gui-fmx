#ifndef TFmeProgressMonitorViewH
#define TFmeProgressMonitorViewH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.TreeView.hpp>
#include <FMX.Types.hpp>
#include <FMX.Objects.hpp>
//---------------------------------------------------------------------------

/// Progress monitor view class
///
class ESCORE_GUI_PKG TFmeProgressMonitorView : public TFrame
{
__published:	// IDE-managed Components
	TTreeView *tvTasks_;
	TText *txtReady_;

private:
	class Handler : public EsEventSubscriber
	{
	public:
		Handler(TFmeProgressMonitorView* This);
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
    TFmeProgressMonitorView* m_this;
	};
	friend class Handler;

public:
	__fastcall TFmeProgressMonitorView(TComponent* Owner);
	virtual __fastcall ~TFmeProgressMonitorView();

  /// Save current height as default height
  void saveDefaultHeight();

	/// Progress monitor interface setter
	void progressMonitorSet(const EsProgressMonitorIntf::Ptr& mon);

  /// Progress monitor view real (unaligned) height
  __property float realHeight = {read=m_realHeight};

	/// Progress monitor events
	///

	/// Event is fired if progress monitor reflectes some activity
	__property TNotifyEvent onActivity = {read=m_onActivity, write=m_onActivity};
	/// Event is fired if progress monitor becomes empty (ready, AKA idle state)
	__property TNotifyEvent onReady = {read=m_onReady, write=m_onReady};

protected:
	// Internal view services
	void viewClear();
	void viewRebuild();
	TTreeViewItem* taskItemFind(const EsString& path, size_t& idx) const;
	TTreeViewItem* parentTaskItemFind(const EsString& path) const;
	TTreeViewItem* taskViewCreate(const EsProgressMonitorTaskIntf::Ptr& task);
	TTreeViewItem* taskViewCreate(const EsString& path,	const EsString& text,
		const EsString& progressText, const EsVariant& completion );
	void taskViewDestroy(const EsString& path);
  void heightUpdate();

	// EsEvent handlers
	void onTaskTextChanged(const EsVariant& payload);
	void onTaskStateChanged(const EsVariant& payload);
	void onTaskProgressChanged(const EsVariant& payload);
	void onTaskAdded(const EsVariant& payload);
	void onTaskRemoved(const EsVariant& payload);

protected:
	Handler m_handler;
  float m_defHeight;
  float m_realHeight;
	EsProgressMonitorIntf::Ptr m_mon;
	EsStringIndexedMap m_items;
	TNotifyEvent m_onActivity;
	TNotifyEvent m_onReady;
};
//---------------------------------------------------------------------------
#endif
