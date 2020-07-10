#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmEsProtocolConfig.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_RPC
//---------------------------------------------------------------------------

#pragma link "TFrmReflectedClassConfig"
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

__fastcall TFrmEsProtocolConfig::TFrmEsProtocolConfig(TComponent* Owner) :
TFrmReflectedClassConfig(Owner, EsRpcMaster::classNameGetStatic()),
m_basicHeight(0),
m_advancedClosedHeight(0),
m_advancedContentsHeight(0)
{
  m_basicHeight = layContents_->Height;
	advanced_->IsExpanded = false;
	m_advancedClosedHeight = advanced_->Height;
  m_advancedContentsHeight = layAdvanced_->Height;
	advanced_->OnExpandedChanged = advanced_Resize;
  advanced_->OnResize = advanced_Resize;

	// set-up property links
	m_src->link( new EsTextCtlPropertyLink(esT("clientAddr"), cbxClientAddr_, lblClientAddr_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("packetTimeout"), edPacketTmo_, lblPacketTmo_) );
	// set-up popular values
	cbxClientAddr_->Items->Add(esT("255"));
	cbxClientAddr_->Items->Add(esT("5"));

  Height = m_basicHeight+m_advancedClosedHeight;

  guiTextUpdate();
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
}
//---------------------------------------------------------------------------

void TFrmEsProtocolConfig::guiTextUpdate()
{
  btnResetToDefaults_->Text = _S("Reset To Defaults");
  advanced_->Text = _S("Advanced");
}
//---------------------------------------------------------------------------

void __fastcall TFrmEsProtocolConfig::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TFrmEsProtocolConfig::btnResetToDefaults_Click(TObject *Sender)
{
	m_src->resetControlsToDefault();
}
//---------------------------------------------------------------------------

float TFrmEsProtocolConfig::totalHeightGet() const
{
  return m_basicHeight + m_advancedClosedHeight +
    (advanced_->IsExpanded ? m_advancedContentsHeight : 0.f);
}
//---------------------------------------------------------------------------

void __fastcall TFrmEsProtocolConfig::advanced_Resize(TObject *Sender)
{
  advanced_->OnExpandedChanged = nullptr;
  advanced_->OnResize = nullptr;

  try
  {
    if( !advanced_->IsExpanded )
      m_advancedClosedHeight = advanced_->Height;
    else
    {
      // As far as advanced grid is showing for the first time, controls in it
      // do not have styles applied, so m_advancedContentsHeight may be miscalculated
      EsUtilities::gridPanelLayoutForceApplyStyle(layAdvanced_);

      advanced_->Height = m_advancedClosedHeight+m_advancedContentsHeight;
    }

    Height = totalHeightGet();
    sizeChangedPost(0, totalHeightGet());
  }
  __finally
  {
    advanced_->OnExpandedChanged = advanced_Resize;
    advanced_->OnResize = advanced_Resize;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFrmEsProtocolConfig::ctl_ApplyStyleLookup(TObject *Sender)
{
  EsUtilities::styledContolHeightAdjust(Sender, 24);

  m_basicHeight = layContents_->Height;
  m_advancedContentsHeight = layAdvanced_->Height;
}
//---------------------------------------------------------------------------


void __fastcall TFrmEsProtocolConfig::advanced_ApplyStyleLookup(TObject *Sender)
{
  TExpanderButton* btn =
    dynamic_cast<TExpanderButton*>(advanced_->FindStyleResource(esT("button")));

  if( btn )
    m_advancedClosedHeight = btn->Height + 2;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_RPC
