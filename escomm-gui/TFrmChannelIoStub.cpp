#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmChannelIoStub.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_CHANNEL_STUB

//---------------------------------------------------------------------------
#pragma link "TFrmReflectedClassConfig"
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

__fastcall TFrmChannelIoStub::TFrmChannelIoStub(TComponent* Owner)
	: TFrmReflectedClassConfig(Owner, EsChannelIoStub::classNameGetStatic())
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();
}
//---------------------------------------------------------------------------

void TFrmChannelIoStub::guiTextUpdate()
{
  lblStub_->Text = _("Stub channel does not have configurable properties");
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoStub::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

float TFrmChannelIoStub::totalHeightGet() const
{
  return lblStub_->Height;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_STUB

