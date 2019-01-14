#ifndef TFrmEsProtocolConfigH
#define TFrmEsProtocolConfigH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.ComboEdit.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include "TFrmReflectedClassConfig.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_RPC

class ESCOMM_GUI_PKG TFrmEsProtocolConfig : public TFrmReflectedClassConfig
{
__published:	// IDE-managed Components
	TExpander *advanced_;
	TButton *btnResetToDefaults_;
	TLabel *lblClientAddr_;
	TComboEdit *cbxClientAddr_;
	TLabel *lblPacketTmo_;
	TNumberBox *edPacketTmo_;
  TGridPanelLayout *layAdvanced_;
	void __fastcall btnResetToDefaults_Click(TObject *Sender);
  void __fastcall ctl_ApplyStyleLookup(TObject *Sender);
  void __fastcall advanced_ApplyStyleLookup(TObject *Sender);

public:		// User declarations
	__fastcall TFrmEsProtocolConfig(TComponent* Owner);

  virtual float totalHeightGet() const;

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
	void __fastcall advanced_Resize(TObject *Sender);

protected:
  float m_basicHeight;
  float m_advancedClosedHeight;
  float m_advancedContentsHeight;
};

//---------------------------------------------------------------------------
#endif // ES_COMM_USE_RPC
#endif
