#ifndef TDlgBaseH
#define TDlgBaseH
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP
# include <functional>
#endif

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
//---------------------------------------------------------------------------

class PACKAGE TDlgBase;
typedef void __fastcall (__closure* EsDlgOnClose)(TDlgBase* Sender, TModalResult mr, const EsVariant& payload);

class ESCORE_GUI_PKG TDlgBase : public TForm
{
__published:	// IDE-managed Components
	TLayout *layContents_;
	TLayout *layBtns_;
	TButton *btnCancel_;
	TButton *btnOk_;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall btn_ApplyStyleLookup(TObject *Sender);

public:		// User declarations
	__fastcall TDlgBase(TComponent* Owner);
	virtual __fastcall ~TDlgBase();

  // Init I18n support
  virtual void i18nInit();

  void showModal(EsDlgOnClose onClose);

#ifdef ES_MODERN_CPP
  void showModal( std::function<void (TDlgBase* Sender, TModalResult mr, const EsVariant& payload)> onClose );
#endif

  void payloadSet(const EsVariant& payload);
  const EsVariant& payloadGet() const;

  /// Set minimal dialog width and height. If 0, minimum extent is not restricted.
  /// If not 0, but less than defXXX value is set, use default instead
  ///
  void setMinWidthHeight(ulong w, ulong h);

protected:
  void formReposition();
  void restrictMinWidthHeight();
  virtual void onResize();

  virtual void onDlgShow();
  virtual void __fastcall onI18nStringsUpdate(TObject* sender);

protected:
	EsI18nManagerScope m_i18nScope;
  EsVariant m_payload;
  ulong m_minW;
  ulong m_minH;

private:
  TDlgBase(const TDlgBase&) ES_REMOVEDECL;
  TDlgBase& operator=(const TDlgBase&) ES_REMOVEDECL;
};

//---------------------------------------------------------------------------
#endif
