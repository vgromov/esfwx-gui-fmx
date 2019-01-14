#ifndef TDlgChannelIoConfigH
#define TDlgChannelIoConfigH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include "TDlgBase.h"
//---------------------------------------------------------------------------

class ESCOMM_GUI_PKG TDlgChannelIoConfig : public TDlgBase
{
__published:	// IDE-managed Components

private:
	/// Events handler
	class EventSubscriber : public EsEventSubscriber
	{
	public:
		EventSubscriber(TDlgChannelIoConfig& owner);
		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		TDlgChannelIoConfig& m_owner;
	};
	friend class EventSubscriber;

protected:
	__fastcall TDlgChannelIoConfig(TComponent* Owner);

public:
  static TDlgChannelIoConfig* create(TComponent* Owner);

	virtual __fastcall ~TDlgChannelIoConfig();

  virtual void i18nInit();

	/// Return copy of configured channel instance
	///
	EsChannelIoIntf::Ptr configuredChannelGet() const;

	/// Set channel for configuration and select corresponding
	/// item from the list
	///
	void channelSet(const EsChannelIoIntf::Ptr& chnl);

protected:
  virtual void init();
  void onPaneSizeChanged(TControl* pane, double dx, double dy);

protected:
  EventSubscriber m_handler;
	TFrmChannelIoConfig* m_paneOwner;
	double m_minWidth;
	double m_minHeight;
};

//---------------------------------------------------------------------------
#endif
