#ifndef TFrmChannelIoConfigH
#define TFrmChannelIoConfigH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Controls.Presentation.hpp>
//---------------------------------------------------------------------------

class ESCOMM_GUI_PKG TFrmChannelIoConfig : public TFrame
{
__published:	// IDE-managed Components
	TLabel *lblChannel_;
	TComboBox *cbxChannel_;
	TLayout *layConfig_;
	TLayout *layContents_;
  TGridPanelLayout *laySelector_;
	void __fastcall cbxChannel_Change(TObject *Sender);
  void __fastcall ctl_ApplyStyleLookup(TObject *Sender);

private:
	/// Events handler
	class EventSubscriber : public EsEventSubscriber
	{
	public:
		EventSubscriber(TFrmChannelIoConfig& owner);
		// event notification interface
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		TFrmChannelIoConfig& m_owner;
	};
	friend class EventSubscriber;

public:
	__fastcall TFrmChannelIoConfig(TComponent* Owner);
	virtual __fastcall ~TFrmChannelIoConfig();

	/// return copy of configured channel instance
	///
	EsChannelIoIntf::Ptr configuredChannelGet() const;

	/// set channel for configuration and select corresponding
	/// item from the list
	///
	void channelSet(const EsChannelIoIntf::Ptr& chnl);

	/// configuration pane access
	///
	TFrmReflectedClassConfig* paneGet() const;

  /// Return total contents height
  float totalHeightGet() const;

  void i18nMgrSet(PlainEsI18nManagerPtrT mgr);

protected:
  void guiTextUpdate();
  void __fastcall onGuiStringsUpdate(TObject* sender);
	void populateChannels();
	void configPaneClose();
	void configPaneCreate(const EsString& chnl);

  void sizeChangedPost(double dx, double dy);
  void paneCreatedPost(TFrmReflectedClassConfig* pane);

  // App events
  //
  void onPaneSizeChanged(TControl* pane, double dx, double dy);

protected:
  EsI18nManagerScope m_i18nScope;
  EventSubscriber m_handler;
	float m_maxChnlStrLen;
	EsString::Array m_channelClasses;
	EsChannelIoIntf::Ptr m_chnl;
	TFrmReflectedClassConfig* m_cfgPane;
  float m_selectorHeight;
};
//---------------------------------------------------------------------------
#endif
