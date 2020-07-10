#ifndef TFrmChannelIoUartH
#define TFrmChannelIoUartH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include "TFrmReflectedClassConfig.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_UART

class ESCOMM_GUI_PKG TFrmChannelIoUart : public TFrmReflectedClassConfig
{
__published:	// IDE-managed Components
	TComboBox *cbxBits_;
	TButton *btnResetToDefaults_;
	TLabel *lblDevice_;
	TComboBox *cbxDevice_;
	TButton *btnRescan_;
	TLabel *lblBaud_;
	TComboBox *cbxBaud_;
	TLabel *lblBits_;
	TLabel *lblParity_;
	TComboBox *cbxParity_;
	TLabel *lblRxBuff_;
	TNumberBox *edRxBuff_;
	TLabel *lblTxBuff_;
	TNumberBox *edTxBuff_;
	TLabel *lblAutoReset_;
	TSwitch *swAutoReset_;
	TExpander *advanced_;
	TLayout *Layout2;
	TLabel *lblStopBit_;
	TComboBox *cbxStopBit_;
  TGridPanelLayout *layAdvanced_;
	void __fastcall btnResetToDefaults_Click(TObject *Sender);
	void __fastcall btnRescan_Click(TObject *Sender);
  void __fastcall ctl_ApplyStyleLookup(TObject *Sender);
  void __fastcall advanced_ApplyStyleLookup(TObject *Sender);

public:		// User declarations
	__fastcall TFrmChannelIoUart(TComponent* Owner);

  virtual float totalHeightGet() const;

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
	void __fastcall advanced_OnResize(TObject *Sender);

  // populate ports selector
	void populatePorts();

protected:
	EsUartEnumerator m_enu;
  float m_basicHeight;
	float m_advancedClosedHeight;
  float m_advancedContentsHeight;
	float m_maxDevStrLen;
};

#endif // ES_COMM_USE_CHANNEL_UART

//---------------------------------------------------------------------------
#endif
