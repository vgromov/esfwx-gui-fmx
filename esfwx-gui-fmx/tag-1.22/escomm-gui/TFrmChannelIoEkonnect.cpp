#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmChannelIoEkonnect.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_CHANNEL_EKONNECT

//---------------------------------------------------------------------------
#pragma link "TFrmReflectedClassConfig"
#pragma resource "*.fmx"

// custom property link for com port enumeration
//
// combo box link
class EsEkonnectIoDevicePropertyLink : public EsReflectedClassPropertyLink
{
public:
	__fastcall EsEkonnectIoDevicePropertyLink(const EsString& propName,
		TComboBox* cbx, TLabel* lbl, const EsFtdiDriver::DeviceList& devices) :
	EsReflectedClassPropertyLink(cbx, propName),
	m_cbx(cbx),
	m_lbl(lbl),
	m_devices(devices)
	{
		ES_ASSERT(m_cbx);
	}

	virtual bool isUnidirectional() const
	{
  	return !m_cbx->Enabled;
	}

protected:
	virtual void initialize()
	{
		if( m_lbl )
			m_lbl->Text = label + esT(":");
	}

	virtual void doControlValSet(const EsVariant& val)
	{
		const EsString& devSerial = val.asString();
		for(size_t idx = 0; idx < m_devices.size(); ++idx)
			if( m_devices[idx].serialNumberStrGet() == devSerial )
			{
				m_cbx->ItemIndex = idx;
				return;
			}

		m_cbx->ItemIndex = -1;
	}

	virtual EsVariant doControlValGet() const
	{
		int sel = m_cbx->ItemIndex;
		if( -1 < sel )
			return m_devices[sel].serialNumberStrGet();

		return EsString::null();
	}

protected:
	TComboBox* m_cbx;
	TLabel* m_lbl;
	const EsFtdiDriver::DeviceList& m_devices;
};

//---------------------------------------------------------------------------
__fastcall TFrmChannelIoEkonnect::TFrmChannelIoEkonnect(TComponent* Owner) :
TFrmReflectedClassConfig(Owner, EsChannelIoEkonnect::classNameGetStatic()),
m_basicHeight(0),
m_advancedClosedHeight(0),
m_advancedContentsHeight(0),
m_maxDevStrLen(0)
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();

  m_basicHeight = layContents_->Height;
	advanced_->IsExpanded = false;
	m_advancedClosedHeight = advanced_->Height;
	advanced_->OnExpandedChanged = advanced_OnResize;
  advanced_->OnResize = advanced_OnResize;
  m_advancedContentsHeight = layAdvanced_->Height;

	populateDevices();
	// set-up property links
	m_src->link( new EsEkonnectIoDevicePropertyLink(esT("device"), cbxDevice_, lblDevice_, m_devices) );
	m_src->link( new EsComboBoxPropertyLink(esT("baud"), cbxBaud_, lblBaud_) );
	m_src->link( new EsSwitchPropertyLink(esT("useRS232"), swUseRS232_, lblUseRS232_) );
	m_src->link( new EsComboBoxPropertyLink(esT("bits"), cbxBits_, lblBits_) );
	m_src->link( new EsComboBoxPropertyLink(esT("parity"), cbxParity_, lblParity_) );
	m_src->link( new EsComboBoxPropertyLink(esT("stopBits"), cbxStopBit_, lblStopBit_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("rxTmo"), edRxTmo_, lblRxTmo_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("txTmo"), edTxTmo_, lblTxTmo_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("rxBuffLen"), edRxBuff_, lblRxBuff_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("txBuffLen"), edTxBuff_, lblTxBuff_) );
	m_src->link( new EsSwitchPropertyLink(esT("resetOnRxTmo"), swAutoReset_, lblAutoReset_) );

	// Update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
  Height = m_basicHeight+m_advancedClosedHeight;
}
//---------------------------------------------------------------------------

void TFrmChannelIoEkonnect::guiTextUpdate()
{
  btnResetToDefaults_->Text = _("Reset To Defaults");
  advanced_->Text = _("Advanced");
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

float TFrmChannelIoEkonnect::totalHeightGet() const
{
  return m_basicHeight + m_advancedClosedHeight +
    (advanced_->IsExpanded ? m_advancedContentsHeight : 0.f);
}
//---------------------------------------------------------------------------

void TFrmChannelIoEkonnect::populateDevices()
{
	EsUpdateLocker<TControl> lock(cbxDevice_);
	cbxDevice_->Clear();
	m_maxDevStrLen = 0;
	// enumerate Ekonnect communication devices on this machine
	m_devices = EsChannelIoEkonnect::getDeviceList();
	// fill-in device selector with user-friendly device names
	for(size_t idx = 0; idx < m_devices.size(); ++idx)
	{
		const EsString& name = m_devices[idx].serialNumberStrGet();
		cbxDevice_->Items->Add( name.c_str() );

		const TRectF& r = EsUtilities::textRectGet(cbxDevice_, name);
		float w = r.Width()+cbxDevice_->Padding->Left+cbxDevice_->Padding->Right+
			24; // consider 24 is the button width
		if( m_maxDevStrLen < w )
			m_maxDevStrLen = w;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::btnResetToDefaults_Click(TObject *Sender)
{
	m_src->resetControlsToDefault();
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::btnRescan_Click(TObject *Sender)
{
	populateDevices();

	// update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::advanced_OnResize(TObject *Sender)
{
  advanced_->OnExpandedChanged = nullptr;
  advanced_->OnResize = nullptr;

  try
  {
    if( !advanced_->IsExpanded )
      m_advancedClosedHeight = advanced_->Height;
    else
    {
      // As far as advanced grid is showing for the first time, controls in it
      // do not have styles applied, so m_advancedContentsHeight may be miscalculated
      EsUtilities::gridPanelLayoutForceApplyStyle(layAdvanced_);

      advanced_->Height = m_advancedClosedHeight+m_advancedContentsHeight;
    }

    Height = totalHeightGet();
    sizeChangedPost(
      0,
      totalHeightGet()
    );
  }
  __finally
  {
    advanced_->OnExpandedChanged = advanced_OnResize;
    advanced_->OnResize = advanced_OnResize;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::ctl_ApplyStyleLookup(TObject *Sender)
{
  EsUtilities::styledContolHeightAdjust(Sender, 24);

  if( btnRescan_ == Sender &&
      btnRescan_->Width < btnRescan_->Height )
    btnRescan_->Width = btnRescan_->Height;

  m_basicHeight = layContents_->Height;
  m_advancedContentsHeight = layAdvanced_->Height;
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoEkonnect::advanced_ApplyStyleLookup(TObject *Sender)
{
  TExpanderButton* btn =
    dynamic_cast<TExpanderButton*>(advanced_->FindStyleResource(esT("button")));

  if( btn )
    m_advancedClosedHeight = btn->Height + 2;
}
//---------------------------------------------------------------------------

#endif // #define ES_COMM_USE_CHANNEL_EKONNECT

