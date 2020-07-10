#include "tests_pch.h"
#pragma hdrstop

#include "TFrmMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TFrmMain *FrmMain;
//---------------------------------------------------------------------------
__fastcall TFrmMain::TFrmMain(TComponent* Owner) :
TForm(Owner)
{
  EsBluetoothLE::initialize();
}
//---------------------------------------------------------------------------

__fastcall TFrmMain::~TFrmMain()
{
  EsBluetoothLE::uninitialize();
}
//---------------------------------------------------------------------------

void __fastcall TFrmMain::Button1Click(TObject *Sender)
{
  EsBluetoothRadioEnumerator btenum;
  btenum.execute();

  const EsBluetoothRadios& radios = btenum.resultGet();
  for(size_t idx = 0; idx < radios.size(); ++idx)
  {
    EsBluetoothRadio::Ptr radio = radios[idx];

    radioInfoLogPrint(radio);

    // Discover LE devices && services
    EsBluetoothLEdeviceEnumerator ledevEnum(radio);
    ledevEnum.execute();

    const EsBluetoothLEdevices& devs = ledevEnum.resultGet();
    for(size_t didx = 0; didx < devs.size(); ++didx )
    {
      EsBluetoothLEdevice::Ptr dev = devs[didx];
      dev->discover();

      leDeviceInfoLog( dev );
    }
  }
}
//---------------------------------------------------------------------------

void TFrmMain::radioInfoLogPrint(const EsBluetoothRadio::Ptr& radio)
{
  EsUpdateLocker<TMemo> lock(log_);
  log_->Lines->Add(esT("---------------"));
  log_->Lines->Add(
    UnicodeString::Format(
      esT("Name: %s"),
      ARRAYOFCONST((
        radio->nameGet().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Address: %s"),
      ARRAYOFCONST((
        radio->addrGet().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Class: %s"),
      ARRAYOFCONST((
        radio->classGet().asString().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Manufacturer: %s"),
      ARRAYOFCONST((
        radio->manufacturerStringGet().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Enabled: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          radio->isEnabled() && radio->isOk()
        ).c_str()
      ))
    )
  );
  log_->Lines->Add(esT("---------------"));
}
//---------------------------------------------------------------------------

void TFrmMain::leDeviceInfoLog(const EsBluetoothLEdevice::Ptr& dev)
{
  log_->Lines->Add(
    UnicodeString::Format(
      esT("Device Name: %s"),
      ARRAYOFCONST((
        dev->nameGet().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Device Addr: %s"),
      ARRAYOFCONST((
        dev->addrGet().c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Device Connected: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          dev->isConnected()
        ).c_str()
      ))
    )
  );

  const EsBluetoothLEservices& svcs = dev->servicesGet();
  for(size_t idx = 0; idx < svcs.size(); ++idx)
  {
    EsBluetoothLEservice::Ptr svc = svcs[idx];
    leServiceInfoLog(svc);
  }
}
//---------------------------------------------------------------------------

void TFrmMain::leServiceInfoLog(const EsBluetoothLEservice::Ptr& svc)
{
  log_->Lines->Add(
    UnicodeString::Format(
      esT("Service is primary: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          svc->isPrimary()
        ).c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Service UUID: {%s}"),
      ARRAYOFCONST((
        EsUtilities::GUIDtoStr(
          svc->uuidGet(),
          true
        ).c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Service is known: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          svc->isKnown()
        ).c_str()
      ))
    )
  );

  if( svc->isKnown() )
  {
    log_->Lines->Add(
      UnicodeString::Format(
        esT("Service name: %s"),
        ARRAYOFCONST((
          svc->knownNameGet().c_str()
        ))
      )
    );
  }

  const EsBluetoothLEcharacteristics& chxs = svc->characteristicsGet();
  for( size_t idx = 0; idx < chxs.size(); ++idx )
  {
    EsBluetoothLEcharacteristic::Ptr chx = chxs[idx];

    leChxInfoLog(chx);
  }
}
//---------------------------------------------------------------------------

void TFrmMain::leChxInfoLog(const EsBluetoothLEcharacteristic::Ptr& chx)
{
  log_->Lines->Add(
    UnicodeString::Format(
      esT("Characteristic UUID: {%s}"),
      ARRAYOFCONST((
        EsUtilities::GUIDtoStr(
          chx->uuidGet(),
          true
        ).c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Characteristic is known: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          chx->isKnown()
        ).c_str()
      ))
    )
  );

  if( chx->isKnown() )
  {
    log_->Lines->Add(
      UnicodeString::Format(
        esT("Characteristic name: %s"),
        ARRAYOFCONST((
          chx->knownNameGet().c_str()
        ))
      )
    );
  }

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Characteristic properties: %s"),
      ARRAYOFCONST((
        chx->propertiesStringGet().c_str()
      ))
    )
  );

  const EsBluetoothLEdescriptors& dcrs = chx->descriptorsGet();
  for(size_t idx = 0; idx < dcrs.size(); ++idx)
  {
    EsBluetoothLEdescriptor::Ptr dcr = dcrs[idx];
    leDcrInfoLog(dcr);
  }
}
//---------------------------------------------------------------------------

void TFrmMain::leDcrInfoLog(const EsBluetoothLEdescriptor::Ptr& dcr)
{
  log_->Lines->Add(
    UnicodeString::Format(
      esT("Descriptor UUID: {%s}"),
      ARRAYOFCONST((
        EsUtilities::GUIDtoStr(
          dcr->uuidGet(),
          true
        ).c_str()
      ))
    )
  );

  log_->Lines->Add(
    UnicodeString::Format(
      esT("Descriptor is known: %s"),
      ARRAYOFCONST((
        EsString::booleanStringGet(
          dcr->isKnown()
        ).c_str()
      ))
    )
  );

  if( dcr->isKnown() )
  {
    log_->Lines->Add(
      UnicodeString::Format(
        esT("Descriptor kind: %s"),
        ARRAYOFCONST((
          dcr->kindNameGet().c_str()
        ))
      )
    );
  }
}
//---------------------------------------------------------------------------

