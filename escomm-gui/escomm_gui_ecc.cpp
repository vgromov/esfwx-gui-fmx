#include "escomm_gui_pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------

USEFORM("TFrmChannelIoStub.cpp", FrmChannelIoStub);
USEFORM("TFrmChannelIoEkonnect.cpp", FrmChannelIoEkonnect);
USEFORM("TFrmChannelIoUart.cpp", FrmChannelIoUart);
USEFORM("TFrmEsProtocolConfig.cpp", FrmEkosfProtocolConfig);
USEFORM("TFrmConnectionConfig.cpp", FrmConnectionConfig);
USEFORM("TFrmChannelIoConfig.cpp", FrmChannelIoConfig);
USEFORM("TDlgChannelIoConfig.cpp", DlgChannelIoConfig);
USEFORM("TFmeChannelIoBluetoothLE.cpp", FmeChannelIoBluetoothLE); /* TFrame: File Type */
USEFORM("TDlgConnectionConfig.cpp", DlgConnectionConfig);
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include "autolink.cxx"
//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------

#pragma argsused
extern "C" int _libmain(unsigned long reason)
{
	return 1;
}
//---------------------------------------------------------------------------
