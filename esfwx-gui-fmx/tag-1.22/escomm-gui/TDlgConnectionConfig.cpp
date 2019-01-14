#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TDlgConnectionConfig.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_RPC

//---------------------------------------------------------------------------
#pragma link "TDlgBase"
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
TDlgConnectionConfig::EventSubscriber::
EventSubscriber(TDlgConnectionConfig& owner) :
EsEventSubscriber(
	EVTC_CLASSCFG_PANE_NOTIFICATION
),
m_owner(owner)
{}
//---------------------------------------------------------------------------

// event notification interface
ES_IMPL_INTF_METHOD(void, TDlgConnectionConfig::EventSubscriber::notify)(const EsEventIntf::Ptr& evt)
{
	const EsString& cat = evt->categoryGet();
	ulong id = evt->idGet();
	const EsVariant& payload = evt->payloadGet();
	if( EVTC_CLASSCFG_PANE_NOTIFICATION == cat )
	{
    switch( static_cast<EsReflectedClassConfigPaneEvent>(id) )
    {
    case EsReflectedClassConfigPaneEvent::SizeChanged:
      {
        TControl* ctl = reinterpret_cast<TControl*>(payload.itemGet(0).asPointer());
        // Handle size events from our proto. and io config panes
        if( m_owner.m_panesOwner == ctl )
        {
          m_owner.onPaneSizeChanged(
            ctl,
            payload.itemGet(1).asDouble(),
            payload.itemGet(2).asDouble()
          );
          evt->consume();
        }
      }
      break;
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TDlgConnectionConfig::TDlgConnectionConfig(TComponent* Owner) :
TDlgBase(Owner),
m_handler(*this),
m_minWidth(0),
m_minHeight(0)
{
}
//---------------------------------------------------------------------------

__fastcall TDlgConnectionConfig::~TDlgConnectionConfig()
{}
//---------------------------------------------------------------------------

TDlgConnectionConfig* TDlgConnectionConfig::create(TComponent* Owner)
{
	ES_ASSERT(Owner);
	TDlgConnectionConfig* dlg = EsUtilities::componentFindExisting<TDlgConnectionConfig>(Owner);
  if( !dlg )
  {
    dlg = new TDlgConnectionConfig(Owner);
	  ES_ASSERT(dlg);
	  dlg->init();
  }

	return dlg;
}
//---------------------------------------------------------------------------

void TDlgConnectionConfig::i18nInit()
{
  TDlgBase::i18nInit();
  ES_ASSERT(m_panesOwner);
  m_panesOwner->i18nMgrSet(
    EsI18nManager::instanceGet()
  );
}
//---------------------------------------------------------------------------

void TDlgConnectionConfig::init()
{
  Name = ClassName();

	m_panesOwner = new TFrmConnectionConfig(this);
	ES_ASSERT(m_panesOwner);
	m_panesOwner->Align = TAlignLayout::Top;

	ClientWidth = m_panesOwner->Width;
	m_minWidth = ClientWidth;
	m_minHeight = ClientHeight;
	m_panesOwner->Parent = layContents_;

  i18nInit();
}
//---------------------------------------------------------------------------

void TDlgConnectionConfig::onPaneSizeChanged(TControl* pane, double x, double y)
{
	ES_ASSERT( pane );
	const TPointF& ext = EsUtilities::contentsExtentGet(pane);

	float newHeight = esMax(m_minHeight, (double)layBtns_->Height + ext.Y );
	float newWidth = esMax(m_minWidth, (double)ext.X );

	if(
    !EsUtilities::areEqualFloats(ClientHeight, newHeight) ||
    !EsUtilities::areEqualFloats(ClientWidth, newWidth)
  )
	{
		ClientHeight = newHeight;
		ClientWidth = newWidth;

		EsUtilities::formPositionFix(this);
	}
}
//---------------------------------------------------------------------------

/// set master protocol object settings to the
/// configuration pane. optionally, channel is selected
/// from the master object as well
///
void TDlgConnectionConfig::masterSet(const EsRpcMasterIntf::Ptr& master)
{
	m_panesOwner->masterSet(master);
}
//---------------------------------------------------------------------------

/// return copy of configured master protocol object
///
EsRpcMasterIntf::Ptr TDlgConnectionConfig::configuredMasterGet() const
{
	return m_panesOwner->configuredMasterGet();
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_RPC

