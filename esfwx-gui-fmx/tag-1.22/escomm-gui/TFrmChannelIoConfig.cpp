#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmChannelIoConfig.h"
#include "TFrmChannelIoStub.h"
#include "TFrmChannelIoUart.h"
#include "TFrmChannelIoEkonnect.h"
#include "TFmeChannelIoBluetoothLE.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

TFrmChannelIoConfig::EventSubscriber::
EventSubscriber(TFrmChannelIoConfig& owner) :
EsEventSubscriber(
EVTC_CLASSCFG_PANE_NOTIFICATION,
200 //< Higher event handling precedence
),
m_owner(owner)
{}
//---------------------------------------------------------------------------

// event notification interface
ES_IMPL_INTF_METHOD(void, TFrmChannelIoConfig::EventSubscriber::notify)(const EsEventIntf::Ptr& evt)
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
        // Handle size events from our nested pane
        TControl* ctl = reinterpret_cast<TControl*>(payload.itemGet(0).asPointer());
        if( m_owner.paneGet() == ctl )
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

__fastcall TFrmChannelIoConfig::TFrmChannelIoConfig(TComponent* Owner) :
TFrame(Owner),
m_i18nScope(this),
m_handler(*this),
m_cfgPane(0),
m_maxChnlStrLen(0),
m_selectorHeight(0)
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();

  m_selectorHeight = laySelector_->Height;
	populateChannels();
	if( !m_channelClasses.empty() )
	{
		// update frame width to show full channel list strings
		if( cbxChannel_->Width < m_maxChnlStrLen )
			Width = Width + m_maxChnlStrLen - cbxChannel_->Width;
		// select first available channel
		cbxChannel_->ItemIndex = 0;
	}
}
//---------------------------------------------------------------------------

__fastcall TFrmChannelIoConfig::~TFrmChannelIoConfig()
{}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::i18nMgrSet(PlainEsI18nManagerPtrT mgr)
{
  m_i18nScope.managerSet(mgr);
  if( m_cfgPane )
    m_cfgPane->i18nMgrSet(mgr);
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::guiTextUpdate()
{
  lblChannel_->Text = _("Select channel:");
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoConfig::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

TFrmReflectedClassConfig* TFrmChannelIoConfig::paneGet() const
{
	return m_cfgPane;
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::populateChannels()
{
	m_maxChnlStrLen = cbxChannel_->Width;
	m_channelClasses = EsChannelIoFactory::channelNamesGet();
	const EsString::Array& chnlDescrs = EsChannelIoFactory::channelDescriptionsGet();
	EsUpdateLocker<TControl> locker(cbxChannel_);
	cbxChannel_->Clear();
	for(size_t idx = 0; idx < chnlDescrs.size(); ++idx)
	{
		const EsString& descr = chnlDescrs[idx];
		cbxChannel_->Items->Add( descr.c_str() );
		const TRectF& r = EsUtilities::textRectGet(cbxChannel_, descr);
		float w = r.Width()+cbxChannel_->Padding->Left+cbxChannel_->Padding->Right+
			24; // consider 24 is the button width
		if( m_maxChnlStrLen < w )
			m_maxChnlStrLen = w;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoConfig::cbxChannel_Change(TObject *Sender)
{
	const EsString& newType = ((-1 < cbxChannel_->ItemIndex) ?
		m_channelClasses[cbxChannel_->ItemIndex] :
		EsString::null());

	const EsString& curType = m_chnl ?
		m_chnl->typeNameGet() :
		EsString::null();

	if( newType != curType )
	{
		configPaneClose();
		m_chnl.reset();

		if( !newType.empty() )
		{
			m_chnl = EsChannelIoFactory::channelCreate(newType);
			configPaneCreate(newType);
		}
	}
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::configPaneClose()
{
	if( m_cfgPane )
	{
    m_cfgPane->i18nMgrSet(nullptr);
    m_cfgPane->Visible = false;
		m_cfgPane->Parent = nullptr;
		ES_DELETE( m_cfgPane );
	}
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::paneCreatedPost(TFrmReflectedClassConfig* pane)
{
  EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
  payload.addToVariantCollection(EsVariant(reinterpret_cast<void*>(this), EsVariant::ACCEPT_POINTER))
    .addToVariantCollection(EsVariant(reinterpret_cast<void*>(pane), EsVariant::ACCEPT_POINTER));

  EsEventDispatcher::eventPost(
    EVTC_CLASSCFG_PANE_NOTIFICATION,
    static_cast<ulong>(EsReflectedClassConfigPaneEvent::PaneCreated),
    payload
  );
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::sizeChangedPost(double dx, double dy)
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

void TFrmChannelIoConfig::configPaneCreate(const EsString& chnl)
{
	ES_ASSERT(!m_cfgPane);
	if( chnl == esT("EsChannelIoStub") )
#ifdef ES_COMM_USE_CHANNEL_STUB
		m_cfgPane = new TFrmChannelIoStub( this );
#else
    EsException::Throw(esT("ES_COMM_USE_CHANNEL_STUB functionality is disabled in this build"));
#endif
	else if( chnl == esT("EsChannelIoUart") )
#ifdef ES_COMM_USE_CHANNEL_UART
		m_cfgPane = new TFrmChannelIoUart( this );
#else
    EsException::Throw(esT("ES_COMM_USE_CHANNEL_UART functionality is disabled in this build"));
#endif
	else if( chnl == esT("EsChannelIoEkonnect") )
#ifdef ES_COMM_USE_CHANNEL_EKONNECT
		m_cfgPane = new TFrmChannelIoEkonnect( this );
#else
    EsException::Throw(esT("ES_COMM_USE_CHANNEL_EKONNECT functionality is disabled in this build"));
#endif
	else if( chnl == esT("EsChannelIoBluetoothLE") )
#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
		m_cfgPane = new TFmeChannelIoBluetoothLE( this );
#else
    EsException::Throw(esT("ES_COMM_USE_CHANNEL_BLUETOOTH_LE functionality is disabled in this build"));
#endif

	ES_ASSERT(m_cfgPane);
	float deltaw = 0;
	if( Width < m_cfgPane->Width )
		deltaw = m_cfgPane->Width - Width;
	m_cfgPane->Align = TAlignLayout::Top;
	m_cfgPane->Parent = layConfig_;
  m_cfgPane->Visible = true;

  Height = totalHeightGet();

  m_cfgPane->i18nMgrSet( EsI18nManager::instanceGet() );

	paneCreatedPost(m_cfgPane);
}
//---------------------------------------------------------------------------

EsChannelIoIntf::Ptr TFrmChannelIoConfig::configuredChannelGet() const
{
	EsReflectedClassIntf::Ptr result;
	if( m_chnl )
	{
		ES_ASSERT(m_cfgPane);
		m_cfgPane->applyToObject(m_chnl);
		// return copy of the configurd channel instance
		result = EsChannelIoFactory::channelCreate(m_chnl->typeNameGet());
		ES_ASSERT(result);
		result->copyFrom(m_chnl);
	}

	return result;
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::channelSet(const EsChannelIoIntf::Ptr& chnl)
{
	int item = -1;
	if( chnl )
	{
		const EsString& type = chnl->typeNameGet();
		for(size_t idx = 0; idx < m_channelClasses.size(); ++idx)
			if( type == m_channelClasses[idx] )
			{
				item = idx;
				break;
			}
	}

	cbxChannel_->ItemIndex = item;
	if( m_chnl )
	{
		ES_ASSERT( m_cfgPane );
		// copy reflected channel settings
		EsReflectedClassIntf::Ptr dest = m_chnl;
		dest->copyFrom(chnl);
		m_cfgPane->applyFromObject(dest);
	}
}
//---------------------------------------------------------------------------

float TFrmChannelIoConfig::totalHeightGet() const
{
  return m_selectorHeight +
    (m_cfgPane ? m_cfgPane->totalHeightGet() : 0);
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoConfig::ctl_ApplyStyleLookup(TObject *Sender)
{
  EsUtilities::styledContolHeightAdjust(Sender, 24);
  m_selectorHeight = laySelector_->Height;
}
//---------------------------------------------------------------------------

void TFrmChannelIoConfig::onPaneSizeChanged(TControl* pane, double x, double y)
{
  ES_ASSERT(paneGet() == pane);

  float newHeight = totalHeightGet();
  Height = newHeight;
  sizeChangedPost(0, newHeight);
}
//---------------------------------------------------------------------------

