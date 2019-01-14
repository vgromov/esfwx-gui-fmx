#ifndef TFrmChannelIoStubH
#define TFrmChannelIoStubH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include "TFrmReflectedClassConfig.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_STUB

class ESCOMM_GUI_PKG TFrmChannelIoStub : public TFrmReflectedClassConfig
{
__published:	// IDE-managed Components
	TLabel *lblStub_;

public:		// User declarations
	__fastcall TFrmChannelIoStub(TComponent* Owner);

  virtual float totalHeightGet() const;

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
};
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_STUB
#endif
