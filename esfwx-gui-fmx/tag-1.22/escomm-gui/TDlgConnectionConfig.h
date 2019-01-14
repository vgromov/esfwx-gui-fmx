#ifndef TDlgConnectionConfigH
#define TDlgConnectionConfigH
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

#ifdef ES_COMM_USE_RPC

class ESCOMM_GUI_PKG TDlgConnectionConfig : public TDlgBase
{
__published:	// IDE-managed Components

private:
	/// Events handler
	class EventSubscriber : public EsEventSubscriber
	{
	public:
		EventSubscriber(TDlgConnectionConfig& owner);
		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		TDlgConnectionConfig& m_owner;
	};
	friend class EventSubscriber;

protected:		// User declarations
	__fastcall TDlgConnectionConfig(TComponent* Owner);

public:
	static TDlgConnectionConfig* create(TComponent* Owner);
	virtual __fastcall ~TDlgConnectionConfig();

  virtual void i18nInit();

	/// Set master protocol object settings to the
	/// configuration pane. optionally, channel is selected
	/// from the master object as well
	///
	void masterSet(const EsRpcMasterIntf::Ptr& master);

	/// Return copy of configured master protocol object
	///
	EsRpcMasterIntf::Ptr configuredMasterGet() const;

protected:
	virtual void init();
	virtual void onPaneSizeChanged(TControl* pane, double dx, double dy);

protected:
  EventSubscriber m_handler;
	TFrmConnectionConfig* m_panesOwner;
	double m_minWidth;
	double m_minHeight;
};

//---------------------------------------------------------------------------
#endif // ES_COMM_USE_RPC
#endif
