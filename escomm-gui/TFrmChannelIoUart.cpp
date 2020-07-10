#include "escomm_gui_pch.h"
#pragma hdrstop

#include "TFrmChannelIoUart.h"

#pragma package(smart_init)

#ifdef ES_COMM_USE_CHANNEL_UART

//---------------------------------------------------------------------------
#pragma link "TFrmReflectedClassConfig"
#pragma resource "*.fmx"

const EsString::const_pointer c_sysPortPrefix = esT("\\\\.\\");

// custom property link for com port enumeration
//
// combo box link
class EsUartIoPortPropertyLink : public EsReflectedClassPropertyLink
{

public:
	__fastcall EsUartIoPortPropertyLink(const EsString& propName,
								TComboBox* cbx, TLabel* lbl, const EsUartEnumerator& enu) :
	EsReflectedClassPropertyLink(cbx, propName),
	m_cbx(cbx),
	m_lbl(lbl),
	m_enu(enu)
	{
		ES_ASSERT(m_cbx);
	}

	virtual bool isUnidirectional() const
	{
  	return !m_cbx->Enabled;
	}

protected:
	virtual void initialize()
	{
		if( m_lbl )
			m_lbl->Text = label + esT(":");
	}

	virtual void doControlValSet(const EsVariant& val)
	{
		EsUartInfo::Ptr info = m_enu.uartInfoFindByName( val.asString() );

		if( info )
		{
      ES_ASSERT( !info->isEmpty() );

			int idx = m_cbx->Items->IndexOf(info->friendlyNameGet().c_str());
			m_cbx->ItemIndex = idx;
		}
		else
		{
			info = m_enu.uartInfoFindByDevPath( val.asString() );
			if( info )
			{
        ES_ASSERT(!info->isEmpty());

				int idx = m_cbx->Items->IndexOf(info->friendlyNameGet().c_str());
				m_cbx->ItemIndex = idx;
			}
			else
				m_cbx->ItemIndex = -1;
		}
	}

	virtual EsVariant doControlValGet() const
	{
		int sel = m_cbx->ItemIndex;
		if( -1 < sel )
			return m_enu.resultGet()[sel]->devicePathGet();

		return EsString::null();
	}

protected:
	TComboBox* m_cbx;
	TLabel* m_lbl;
	const EsUartEnumerator& m_enu;
};

//---------------------------------------------------------------------------
__fastcall TFrmChannelIoUart::TFrmChannelIoUart(TComponent* Owner) :
TFrmReflectedClassConfig(Owner, EsChannelIoUart::classNameGetStatic()),
m_basicHeight(0),
m_advancedClosedHeight(0),
m_advancedContentsHeight(0),
m_maxDevStrLen(0)
{
  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  guiTextUpdate();

  m_basicHeight = layContents_->Height;
	advanced_->IsExpanded = false;
	m_advancedClosedHeight = advanced_->Height;
  m_advancedContentsHeight = layAdvanced_->Height;
	advanced_->OnExpandedChanged = advanced_OnResize;
  advanced_->OnResize = advanced_OnResize;

	populatePorts();
	// set-up property links
	m_src->link( new EsUartIoPortPropertyLink(esT("port"), cbxDevice_, lblDevice_, m_enu) );
	m_src->link( new EsComboBoxPropertyLink(esT("baud"), cbxBaud_, lblBaud_) );
	m_src->link( new EsComboBoxPropertyLink(esT("bits"), cbxBits_, lblBits_) );
	m_src->link( new EsComboBoxPropertyLink(esT("parity"), cbxParity_, lblParity_) );
	m_src->link( new EsComboBoxPropertyLink(esT("stopBits"), cbxStopBit_, lblStopBit_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("rxBuffLen"), edRxBuff_, lblRxBuff_) );
	m_src->link( new EsSpinCtlPropertyLink(esT("txBuffLen"), edTxBuff_, lblTxBuff_) );
	m_src->link( new EsSwitchPropertyLink(esT("resetOnRxTmo"), swAutoReset_, lblAutoReset_) );

	// Update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
  Height = m_basicHeight+m_advancedClosedHeight;
}
//---------------------------------------------------------------------------

void TFrmChannelIoUart::guiTextUpdate()
{
  btnResetToDefaults_->Text = _S("Reset To Defaults");
  advanced_->Text = _S("Advanced");
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::onGuiStringsUpdate(TObject* sender)
{
  guiTextUpdate();
}
//---------------------------------------------------------------------------

float TFrmChannelIoUart::totalHeightGet() const
{
  return m_basicHeight + m_advancedClosedHeight +
    (advanced_->IsExpanded ? m_advancedContentsHeight : 0.f);
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::btnResetToDefaults_Click(TObject *Sender)
{
	m_src->resetControlsToDefault();
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::btnRescan_Click(TObject *Sender)
{
	populatePorts();

	// update frame width to show full channel list strings
	if( cbxDevice_->Width < m_maxDevStrLen )
		Width = Width + m_maxDevStrLen - cbxDevice_->Width;
}
//---------------------------------------------------------------------------

// populate port selector
void TFrmChannelIoUart::populatePorts()
{
	EsUpdateLocker<TControl> lock(cbxDevice_);
	cbxDevice_->Clear();
	m_maxDevStrLen = 0;
	// enumerate communication ports on this machine, ignoring busy ones
	m_enu.enumerate(true);
  const EsUartInfos& infos = m_enu.resultGet();
	// fill-in port selector with user-friendly port names, ignoring modems
	for(size_t idx = 0; idx < infos.size(); ++idx)
	{
		if( infos[idx]->isModem() )
			continue;

		const EsString& name = infos[idx]->friendlyNameGet();
		cbxDevice_->Items->Add(name.c_str());

		const TRectF& r = EsUtilities::textRectGet(cbxDevice_, name);
		float w = r.Width()+cbxDevice_->Padding->Left+cbxDevice_->Padding->Right+
			24; // consider 24 is the button width
		if( m_maxDevStrLen < w )
			m_maxDevStrLen = w;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::advanced_OnResize(TObject *Sender)
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
    advanced_->OnExpandedChanged = advanced_OnResize;
    advanced_->OnResize = advanced_OnResize;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::ctl_ApplyStyleLookup(TObject *Sender)
{
  EsUtilities::styledContolHeightAdjust(Sender, 24);

  if( btnRescan_ == Sender &&
      btnRescan_->Width < btnRescan_->Height )
    btnRescan_->Width = btnRescan_->Height;

  m_basicHeight = layContents_->Height;
  m_advancedContentsHeight = layAdvanced_->Height;
}
//---------------------------------------------------------------------------

void __fastcall TFrmChannelIoUart::advanced_ApplyStyleLookup(TObject *Sender)
{
  TExpanderButton* btn =
    dynamic_cast<TExpanderButton*>(advanced_->FindStyleResource(esT("button")));

  if( btn )
    m_advancedClosedHeight = btn->Height + 2;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_UART

