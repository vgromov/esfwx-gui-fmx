#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmConnectionConfig.h"
#include "TFrmEsProtocolConfig.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_RPC

//---------------------------------------------------------------------------
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
TFrmConnectionConfig::EventSubscriber::
EventSubscriber(TFrmConnectionConfig& owner) :
EsEventSubscriber(
	EVTC_CLASSCFG_PANE_NOTIFICATION,
  500 //< Higher event handling precedence, but lower, than nested IOconfig pane
),
m_owner(owner)
{}
//---------------------------------------------------------------------------

// event notification interface
ES_IMPL_INTF_METHOD(void, TFrmConnectionConfig::EventSubscriber::notify)(const EsEventIntf::Ptr& evt)
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
        if( m_owner.m_protCfg == ctl ||
            m_owner.m_chnlCfg == ctl )
        {
          m_owner.onPaneSizeChanged(
            ctl,
            payload.itemGet(1).asDouble(),
            payload.itemGet(2).asDouble()
          );
        }
      }
      break;
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TFrmConnectionConfig::TFrmConnectionConfig(TComponent* Owner) :
TFrame(Owner),
m_i18nScope(this),
m_handler(*this),
m_protCfg(nullptr),
m_chnlCfg(nullptr)
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();

	m_master = EsRpcMaster::create();
	ES_ASSERT(m_master);

	m_protCfg = new TFrmEsProtocolConfig(this);
	ES_ASSERT(m_protCfg);
	double newWidth = m_protCfg->Width + gbxProtocol_->Padding->Left + gbxProtocol_->Padding->Right;
	Width = newWidth;

	m_protCfg->Parent = gbxProtocol_;
	m_protCfg->Align = TAlignLayout::Top;
	float height = m_protCfg->totalHeightGet();
	gbxProtocol_->Height = height + gbxProtocol_->Padding->Top + gbxProtocol_->Padding->Bottom;

	m_chnlCfg = new TFrmChannelIoConfig(this);
	ES_ASSERT(m_chnlCfg);
	newWidth = m_chnlCfg->Width + gbxChannel_->Padding->Left + gbxChannel_->Padding->Right;
	if(Width < newWidth)
		Width = newWidth;

	m_chnlCfg->Parent = gbxChannel_;
	m_chnlCfg->Align = TAlignLayout::Top;
	onChnlPaneCreated(m_chnlCfg->paneGet());
}
//---------------------------------------------------------------------------

__fastcall TFrmConnectionConfig::~TFrmConnectionConfig()
{}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::guiTextUpdate()
{
  gbxProtocol_->Text = _("Protocol:");
  gbxChannel_->Text = _("Channel:");
}
//---------------------------------------------------------------------------

void __fastcall TFrmConnectionConfig::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::i18nMgrSet(PlainEsI18nManagerPtrT mgr)
{
  m_i18nScope.managerSet(mgr);
  m_protCfg->i18nMgrSet(mgr);
  m_chnlCfg->i18nMgrSet(mgr);
}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::onChnlPaneCreated(TControl* ctl)
{
	TFrmReflectedClassConfig* pane = dynamic_cast<TFrmReflectedClassConfig*>(ctl);
	if( pane )
	{
		gbxChannel_->Width = pane->Width + gbxChannel_->Padding->Left + gbxChannel_->Padding->Right;
		gbxChannel_->Height = pane->totalHeightGet() +	m_chnlCfg->laySelector_->Height +
			gbxChannel_->Padding->Top + gbxChannel_->Padding->Bottom;
	}
}

//---------------------------------------------------------------------------

void TFrmConnectionConfig::onPaneSizeChanged(TControl* pane, double x, double y)
{
  if( m_chnlCfg == pane )
    onChnlContentsResized(x, y);
  else if( m_protCfg == pane )
    onProtContentsResized(x, y);

  Height = totalHeightGet();
  sizeChangedPost(0, totalHeightGet());
}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::sizeChangedPost(double dx, double dy)
{
  EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
  payload.addToVariantCollection(EsVariant(reinterpret_cast<void*>(this), EsVariant::ACCEPT_POINTER))
    .addToVariantCollection(dx)
    .addToVariantCollection(dy);

  EsEventDispatcher::eventPost(
    EVTC_CLASSCFG_PANE_NOTIFICATION,
    static_cast<ulong>(EsReflectedClassConfigPaneEvent::SizeChanged),
    payload
  );
}
//---------------------------------------------------------------------------

float TFrmConnectionConfig::totalHeightGet() const
{
  return gbxProtocol_->Height + gbxChannel_->Height;
}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::onChnlContentsResized(double w, double h)
{
	gbxChannel_->Height = gbxChannel_->Padding->Top + gbxChannel_->Padding->Bottom + h;
}
//---------------------------------------------------------------------------

void TFrmConnectionConfig::onProtContentsResized(double w, double h)
{
	gbxProtocol_->Height = gbxProtocol_->Padding->Top + gbxProtocol_->Padding->Bottom + h;
  gbxChannel_->Position->Y = gbxProtocol_->Position->Y + gbxProtocol_->Height+1;
}
//---------------------------------------------------------------------------

TFrmEsProtocolConfig* TFrmConnectionConfig::protConfigFrameGet() const
{
	return m_protCfg;
}

//---------------------------------------------------------------------------
TFrmChannelIoConfig* TFrmConnectionConfig::chnlConfigFrameGet() const
{
	return m_chnlCfg;
}

//---------------------------------------------------------------------------
/// set master protocol object settings to the
/// configuration pane. optionally, channel is selected
/// from the master object as well
///
void TFrmConnectionConfig::masterSet(const EsRpcMasterIntf::Ptr& master)
{
	if( master )
	{
		EsReflectedClassIntf::Ptr ref = m_master;
		ES_ASSERT(ref);
		ref->copyFrom(master);
		m_protCfg->applyFromObject(m_master);
		m_chnlCfg->channelSet(master->channelGet());
	}
}

//---------------------------------------------------------------------------
/// return copy of configured master protocol object
///
EsRpcMasterIntf::Ptr TFrmConnectionConfig::configuredMasterGet() const
{
	m_protCfg->applyToObject(m_master);

	EsRpcMasterIntf::Ptr tmp = EsRpcMaster::create();
	ES_ASSERT(tmp);
	EsReflectedClassIntf::Ptr ref = tmp;
	ES_ASSERT(ref);
	ref->copyFrom(m_master);
	tmp->channelSet( m_chnlCfg->configuredChannelGet() );

	return tmp;
}
//---------------------------------------------------------------------------

void __fastcall TFrmConnectionConfig::gbx_ApplyStyleLookup(TObject *Sender)
{
  TGroupBox* gbx = dynamic_cast<TGroupBox*>(Sender);

  if( gbx )
  {
    TText* txt = dynamic_cast<TText*>(gbx->FindStyleResource(esT("text")));
    if( txt )
    {
      float oldPadding = gbx->Padding->Top;
      gbx->Padding->Top = txt->Height+2;
      gbx->Height = gbx->Height + gbx->Padding->Top - oldPadding;
    }
  }
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_RPC
