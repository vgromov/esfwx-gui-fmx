#ifndef TFrmReflectedClassConfigH
#define TFrmReflectedClassConfigH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
//---------------------------------------------------------------------------

/// Configuration pane events category
///
#define EVTC_CLASSCFG_PANE_NOTIFICATION esT("fmeReflectedClassConfigPane")

enum class EsReflectedClassConfigPaneEvent : ulong
{
  SizeChanged = 1,  ///< payload[0] contains pointer to pane instance, which size was changed,
                    ///  payload[1] = x (double), payload[2] = y (double)
  PaneCreated = 2,  ///< payload[0] contains pointer to event source instance (sender),
                    ///  payload[1] = pinter to created pane instance
};

/// Base class for reflected objects configuration panes
///
class ESCORE_GUI_PKG TFrmReflectedClassConfig : public TFrame
{
__published:	// IDE-managed Components
  TGridPanelLayout *layContents_;

public:		// User declarations
	__fastcall TFrmReflectedClassConfig(TComponent* Owner, const EsString& className,
								const EsMetaclassIntf::Ptr& meta = EsMetaclassIntf::Ptr());

  virtual __fastcall ~TFrmReflectedClassConfig();

	EsString supportedTypeNameGet() const;
	void applyFromObject(const EsReflectedClassIntf::Ptr& obj);
	void applyToObject(const EsReflectedClassIntf::Ptr& obj);

  EsReflectedClassDataSource& sourceGet();

	// Return total height of frame contents
	virtual float totalHeightGet() const = 0;

  void i18nMgrSet(PlainEsI18nManagerPtrT mgr);

  // True, while setting control properties from the reflected object.
  // False otherwise
  //
  __property bool applyingFromObject = {read=m_applyingFromObject};

protected:
  void sizeChangedPost(double x, double y);

  void __fastcall onGuiStringsUpdate(TObject* sender);

	virtual void doApplyFromObject(const EsReflectedClassIntf::Ptr& obj);
	virtual void doApplyToObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void i18nStringsUpdate(const EsString& loc);

protected:
  EsI18nManagerScope m_i18nScope;
	EsReflectedClassDataSource* m_src;
  bool m_applyingFromObject;

private:
  // Win64 ilink64 'Unresolved external' work-around
  TFrmReflectedClassConfig(const TFrmReflectedClassConfig&) ES_REMOVEDECL;
  TFrmReflectedClassConfig& operator=(const TFrmReflectedClassConfig&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------
#endif
