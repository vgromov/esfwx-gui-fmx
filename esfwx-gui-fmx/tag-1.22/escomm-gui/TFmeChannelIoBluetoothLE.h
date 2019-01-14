#ifndef TFmeChannelIoBluetoothLEH
#define TFmeChannelIoBluetoothLEH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Objects.hpp>
#include <FMX.StdCtrls.hpp>
#include "TFrmReflectedClassConfig.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE

/// Device enumeration for BLE ECO-E byte streaming device
///
class EsEcoEbluetoothLEdeviceEnumerator : public EsBluetoothLEdeviceEnumerator
{
public:
  EsEcoEbluetoothLEdeviceEnumerator();

protected:
  virtual bool onDeviceEnumerated(const EsBluetoothLEdevice::Ptr& device);
  static EsBluetoothRadio::Ptr defaultRadioGet();
};

/// BLE ECO-E byte streaming channel configuration
///
class ESCOMM_GUI_PKG TFmeChannelIoBluetoothLE : public TFrmReflectedClassConfig
{
__published:	// IDE-managed Components
  TButton *btnResetToDefaults_;
  TLayout *Layout2;
  TComboBox *cbxDevice_;
  TButton *btnRescan_;
  TLabel *lblDevice_;
  TLabel *lblSubscribeToNotifications_;
  TSwitch *swSubscribeToNotifications_;
  void __fastcall btnRescan_Click(TObject *Sender);
  void __fastcall ctl_ApplyStyleLookup(TObject *Sender);

public:
  __fastcall TFmeChannelIoBluetoothLE(TComponent* Owner);

  virtual float totalHeightGet() const { return m_basicHeight; }

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
  /// Populate BLE devices selector
	void populateDevices();

protected:
  float m_maxDevStrLen;
  float m_basicHeight;
  EsEcoEbluetoothLEdeviceEnumerator m_enu;
};

#endif // ES_COMM_USE_CHANNEL_BLUETOOTH_LE

//---------------------------------------------------------------------------
#endif
