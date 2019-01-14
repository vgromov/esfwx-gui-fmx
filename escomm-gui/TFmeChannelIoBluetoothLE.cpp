#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFmeChannelIoBluetoothLE.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
//---------------------------------------------------------------------------

#pragma link "TFrmReflectedClassConfig"
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// Custom property link for the ECO-E device enumeration and selection
//
class EsBluetoothLedevicePropertyLink : public EsReflectedClassPropertyLink
{

public:
	__fastcall EsBluetoothLedevicePropertyLink(const EsString& propName,
								TComboBox* cbx, TLabel* lbl, const EsEcoEbluetoothLEdeviceEnumerator& enu) :
	EsReflectedClassPropertyLink(cbx, propName),
	m_cbx(cbx),
	m_lbl(lbl),
	m_enu(enu)
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
		EsBluetoothLEdevice::Ptr dev = m_enu.deviceFindByAddress( val.asString() );

		if( dev )
		{
			int idx = m_cbx->Items->IndexOf( dev->nameGet().c_str() );
			m_cbx->ItemIndex = idx;
		}
		else
			m_cbx->ItemIndex = -1;
	}

	virtual EsVariant doControlValGet() const
	{
		int sel = m_cbx->ItemIndex;
		if( -1 < sel )
			return m_enu.resultGet()[sel]->addrGet();

		return EsBluetooth::anyAddr();
	}

protected:
	TComboBox* m_cbx;
	TLabel* m_lbl;
	const EsEcoEbluetoothLEdeviceEnumerator& m_enu;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsEcoEbluetoothLEdeviceEnumerator::EsEcoEbluetoothLEdeviceEnumerator() :
EsBluetoothLEdeviceEnumerator( defaultRadioGet() )
{
}
//---------------------------------------------------------------------------

EsBluetoothRadio::Ptr EsEcoEbluetoothLEdeviceEnumerator::defaultRadioGet()
{
  EsBluetoothRadioEnumerator renu;
  renu.execute();

  const EsBluetoothRadios& radios = renu.resultGet();
  if( !radios.empty() )
    return radios[0];

  return EsBluetoothRadio::Ptr();
}
//---------------------------------------------------------------------------

bool EsEcoEbluetoothLEdeviceEnumerator::onDeviceEnumerated(const EsBluetoothLEdevice::Ptr& device)
{
  ES_ASSERT( device );
  device->discover();

  EsBluetoothLEservice::Ptr devInfo = device->serviceFind(
    EsBluetoothUUID::fromShortened(0x180A)
  );

  EsBluetoothLEservice::Ptr io = device->serviceFind(
    EsChannelIoBluetoothLE::byteSvcUUID()
  );

  if( devInfo && io )
  {
    EsBluetoothLEcharacteristic::Ptr mfg = devInfo->characteristicFind(
      EsBluetoothUUID::fromShortened(0x2A29)
    );

    if( mfg )
    {
      const EsBinBuffer& bb = mfg->valueGet();
      EsByteString bs( bb.begin(), bb.end() );
      if( esT("ECO-E") == EsString::fromUtf8(bs) )
        m_devs.push_back( device );
    }
  }

  return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

__fastcall TFmeChannelIoBluetoothLE::TFmeChannelIoBluetoothLE(TComponent* Owner) :
TFrmReflectedClassConfig(Owner, EsChannelIoBluetoothLE::classNameGetStatic()),
m_maxDevStrLen(0),
m_basicHeight(0)
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();

	populateDevices();

  m_basicHeight = layContents_->Height;

	// set-up property links
	m_src->link( new EsBluetoothLedevicePropertyLink(esT("deviceAddress"), cbxDevice_, lblDevice_, m_enu) );
	m_src->link( new EsSwitchPropertyLink(esT("subscribeToNotifications"), swSubscribeToNotifications_, lblSubscribeToNotifications_) );

	// Update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
}
//---------------------------------------------------------------------------

void TFmeChannelIoBluetoothLE::guiTextUpdate()
{
  btnResetToDefaults_->Text = _("Reset To Defaults");
}
//---------------------------------------------------------------------------

void __fastcall TFmeChannelIoBluetoothLE::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TFmeChannelIoBluetoothLE::btnRescan_Click(TObject *Sender)
{
	populateDevices();

	// update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
}
//---------------------------------------------------------------------------

void TFmeChannelIoBluetoothLE::populateDevices()
{
	EsUpdateLocker<TControl> lock(cbxDevice_);
	cbxDevice_->Clear();
	m_maxDevStrLen = 0;
	/// (Re)-enumerate BLE devices
	m_enu.execute();
  const EsBluetoothLEdevices& devs = m_enu.resultGet();
	// fill-in port selector with user-friendly port names, ignoring modems
	for(size_t idx = 0; idx < devs.size(); ++idx)
	{
		const EsBluetoothLEdevice::Ptr& dev = devs[idx];
    ES_ASSERT(dev);

    const EsString& name = dev->nameGet();
		cbxDevice_->Items->Add(name.c_str());

		const TRectF& r = EsUtilities::textRectGet(cbxDevice_, name);
		float w = r.Width()+cbxDevice_->Padding->Left+cbxDevice_->Padding->Right+
			24; // consider 24 is the button width
		if( m_maxDevStrLen < w )
			m_maxDevStrLen = w;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmeChannelIoBluetoothLE::ctl_ApplyStyleLookup(TObject *Sender)
{
  EsUtilities::styledContolHeightAdjust(Sender, 24);

  if( btnRescan_ == Sender &&
      btnRescan_->Width < btnRescan_->Height )
    btnRescan_->Width = btnRescan_->Height;

  m_basicHeight = layContents_->Height;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_BLUETOOTH_LE

