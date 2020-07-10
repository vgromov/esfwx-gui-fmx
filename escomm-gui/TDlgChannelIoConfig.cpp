#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TDlgChannelIoConfig.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "TDlgBase"
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

TDlgChannelIoConfig::EventSubscriber::
EventSubscriber(TDlgChannelIoConfig& owner) :
EsEventSubscriber(
	EVTC_CLASSCFG_PANE_NOTIFICATION
),
m_owner(owner)
{}
//---------------------------------------------------------------------------

// event notification interface
ES_IMPL_INTF_METHOD(void, TDlgChannelIoConfig::EventSubscriber::notify)(const EsEventIntf::Ptr& evt)
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
        if( m_owner.m_paneOwner == ctl )
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

__fastcall TDlgChannelIoConfig::TDlgChannelIoConfig(TComponent* Owner) :
TDlgBase(Owner),
m_handler(*this),
m_paneOwner(0),
m_minWidth(0),
m_minHeight(0)
{
}
//---------------------------------------------------------------------------

__fastcall TDlgChannelIoConfig::~TDlgChannelIoConfig()
{}
//---------------------------------------------------------------------------

TDlgChannelIoConfig* TDlgChannelIoConfig::create(TComponent* Owner)
{
	ES_ASSERT(Owner);
	TDlgChannelIoConfig* dlg = EsUtilities::componentFindExisting<TDlgChannelIoConfig>(Owner);
	if( !dlg )
  {
    dlg = new TDlgChannelIoConfig(Owner);
    ES_ASSERT(dlg);
  	dlg->init();
  }

	return dlg;
}
//---------------------------------------------------------------------------

void TDlgChannelIoConfig::i18nInit()
{
  TDlgBase::i18nInit();

  ES_ASSERT(m_paneOwner);
  m_paneOwner->i18nMgrSet(
    EsI18nManager::instanceGet()
  );
}
//---------------------------------------------------------------------------

void TDlgChannelIoConfig::init()
{
  Name = ClassName();

	m_paneOwner = new TFrmChannelIoConfig(this);
	ES_ASSERT(m_paneOwner);
	m_paneOwner->Align = TAlignLayout::Top;

	ClientWidth = m_paneOwner->Width;
	m_minWidth = ClientWidth;
	m_minHeight = ClientHeight;

	m_paneOwner->Parent = layContents_;

  i18nInit();
}
//---------------------------------------------------------------------------

EsChannelIoIntf::Ptr TDlgChannelIoConfig::configuredChannelGet() const
{
	return m_paneOwner->configuredChannelGet();
}
//---------------------------------------------------------------------------

void TDlgChannelIoConfig::channelSet(const EsChannelIoIntf::Ptr& chnl)
{
	m_paneOwner->channelSet(chnl);
}
//---------------------------------------------------------------------------

//void __fastcall TDlgChannelIoConfig::onPaneCreated(TObject* Sender)
//{
//	TFrmReflectedClassConfig* pane = dynamic_cast<TFrmReflectedClassConfig*>(Sender);
//	if( pane )
//	{
//		const TPointF& ext = pane->contentsExtentGet();
//		ClientWidth = esMax(m_minWidth, (double)ext.X);
//		ClientHeight = esMax(m_minHeight, (double)ext.Y +
//			layBtns_->Height +
//			m_paneOwner->laySelector_->Height);
//	}
//}
////---------------------------------------------------------------------------

void TDlgChannelIoConfig::onPaneSizeChanged(TControl* pane, double x, double y)
{
	float newHeight = y + layBtns_->Height;

	if( !EsUtilities::areEqualFloats(ClientHeight, newHeight) )
	{
		ClientHeight = newHeight;
		EsUtilities::formPositionFix(this);
	}
}
//---------------------------------------------------------------------------
