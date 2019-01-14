//---------------------------------------------------------------------------

#ifndef TFrmMainH
#define TFrmMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Types.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Memo.hpp>
#include <FMX.ScrollBox.hpp>
//---------------------------------------------------------------------------
class TFrmMain : public TForm
{
__published:	// IDE-managed Components
	TStyleBook *styleTest_;
	TButton *Button1;
  TMemo *log_;
  TLayout *Layout1;
	void __fastcall Button1Click(TObject *Sender);

public:		// User declarations
	__fastcall TFrmMain(TComponent* Owner);
  virtual __fastcall ~TFrmMain();

protected:
  void radioInfoLogPrint(const EsBluetoothRadio::Ptr& radio);
  void leDeviceInfoLog(const EsBluetoothLEdevice::Ptr& dev);
  void leServiceInfoLog(const EsBluetoothLEservice::Ptr& svc);
  void leChxInfoLog(const EsBluetoothLEcharacteristic::Ptr& chx);
  void leDcrInfoLog(const EsBluetoothLEdescriptor::Ptr& dcr);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmMain *FrmMain;
//---------------------------------------------------------------------------
#endif
