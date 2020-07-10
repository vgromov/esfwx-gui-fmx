#ifndef TFrmChannelIoEkonnectH
#define TFrmChannelIoEkonnectH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Edit.hpp>
#include <FMX.Objects.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include "TFrmReflectedClassConfig.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_EKONNECT

class ESCOMM_GUI_PKG TFrmChannelIoEkonnect : public TFrmReflectedClassConfig
{
__published:	// IDE-managed Components
	TButton *btnResetToDefaults_;
	TLabel *lblDevice_;
	TComboBox *cbxDevice_;
	TButton *btnRescan_;
	TLabel *lblBaud_;
	TComboBox *cbxBaud_;
	TExpander *advanced_;
	TLabel *lblBits_;
	TComboBox *cbxBits_;
	TLabel *lblParity_;
	TComboBox *cbxParity_;
	TLabel *lblStopBit_;
	TComboBox *cbxStopBit_;
	TLabel *lblRxBuff_;
	TNumberBox *edRxBuff_;
	TLabel *lblTxBuff_;
	TNumberBox *edTxBuff_;
	TLabel *lblAutoReset_;
	TSwitch *swAutoReset_;
	TLabel *lblUseRS232_;
	TSwitch *swUseRS232_;
	TLabel *lblRxTmo_;
	TNumberBox *edRxTmo_;
	TLabel *lblTxTmo_;
	TNumberBox *edTxTmo_;
	TLayout *Layout2;
  TGridPanelLayout *layAdvanced_;
	void __fastcall btnResetToDefaults_Click(TObject *Sender);
	void __fastcall btnRescan_Click(TObject *Sender);
  void __fastcall ctl_ApplyStyleLookup(TObject *Sender);
  void __fastcall advanced_ApplyStyleLookup(TObject *Sender);

public:		// User declarations
	__fastcall TFrmChannelIoEkonnect(TComponent* Owner);

  virtual float totalHeightGet() const;

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
	void __fastcall advanced_OnResize(TObject *Sender);

	void populateDevices();

private:
	// channel property links collection
	EsFtdiDriver::DeviceList m_devices;
  float m_basicHeight;
	float m_advancedClosedHeight;
  float m_advancedContentsHeight;
	float m_maxDevStrLen;
};
//---------------------------------------------------------------------------

#endif // #define ES_COMM_USE_CHANNEL_EKONNECT

//---------------------------------------------------------------------------
#endif
