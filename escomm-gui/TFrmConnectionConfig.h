#ifndef TFrmConnectionConfigH
#define TFrmConnectionConfigH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_RPC

// forward decl
class PACKAGE TFrmEsProtocolConfig;

//---------------------------------------------------------------------------
class ESCOMM_GUI_PKG TFrmConnectionConfig : public TFrame
{
__published:	// IDE-managed Components
	TLayout *layContents_;
	TGroupBox *gbxProtocol_;
	TGroupBox *gbxChannel_;
  void __fastcall gbx_ApplyStyleLookup(TObject *Sender);

private:
	/// Events handler
	class EventSubscriber : public EsEventSubscriber
	{
	public:
		EventSubscriber(TFrmConnectionConfig& owner);
		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		TFrmConnectionConfig& m_owner;
	};
	friend class EventSubscriber;

public:
	__fastcall TFrmConnectionConfig(TComponent* Owner);
	virtual __fastcall ~TFrmConnectionConfig();

	/// protocol configuration frame access
	///
	TFrmEsProtocolConfig* protConfigFrameGet() const;

	/// channel configuration frame access
	///
	TFrmChannelIoConfig* chnlConfigFrameGet() const;

	/// set master protocol object settings to the
	/// configuration pane. optionally, channel is selected
	/// from the master object as well
	///
	void masterSet(const EsRpcMasterIntf::Ptr& master);

	/// return copy of configured master protocol object
	///
  EsRpcMasterIntf::Ptr configuredMasterGet() const;

  /// Get total height
  float totalHeightGet() const;

  /// Assign i18n manager
  void i18nMgrSet(PlainEsI18nManagerPtrT mgr);

protected:
	void onChnlContentsResized(double deltaW, double deltaH);
	void onProtContentsResized(double deltaW, double deltaH);
  void sizeChangedPost(double dx, double dy);

  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);

  // App event handlers
  //
  void onPaneSizeChanged(TControl* pane, double dx, double dy);
	void onChnlPaneCreated(TControl* ctl);

private:
  EventSubscriber m_handler;
  EsI18nManagerScope m_i18nScope;
	TFrmEsProtocolConfig* m_protCfg;
	TFrmChannelIoConfig* m_chnlCfg;
	EsRpcMasterIntf::Ptr m_master;
};

//---------------------------------------------------------------------------
#endif // ES_COMM_USE_RPC
#endif
