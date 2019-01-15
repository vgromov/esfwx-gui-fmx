#ifndef _es_core_gui_h_
#define _es_core_gui_h_

#ifndef _es_core_h_
#	error escore.h header file must be included prior to this header!
#endif

#include <EsCoreGuiConfig.h>

// escore export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCOREGUI_EXPORTS
#		define ESCORE_GUI_CLASS	            ES_EXPORT_SPEC
#		define ESCORE_GUI_FUNC(type, decl)	ES_FUNCEXPORT_SPEC(type, decl)
#		define ESCORE_GUI_DATA(type, decl)	ES_DATAEXPORT_SPEC(type, decl)
#		define ESCORE_GUI_PKG		            PACKAGE
#	else
#		define ESCORE_GUI_CLASS	            ES_IMPORT_SPEC
#		define ESCORE_GUI_FUNC(type, decl)	ES_FUNCIMPORT_SPEC(type, decl)
#		define ESCORE_GUI_DATA(type, decl)	ES_DATAIMPORT_SPEC(type, decl)
#		define ESCORE_GUI_PKG		            PACKAGE
#	endif
#else
#	define ESCORE_GUI_CLASS
#	define ESCORE_GUI_FUNC(type, decl)    type decl
#	define ESCORE_GUI_DATA(type, decl)    type decl
#	define ESCORE_GUI_PKG
#endif

// system|rtl stuff
#include <System.hpp>
#include <System.Rtti.hpp>
#include <System.Classes.hpp>
#include <fmx.h>

/// Common typedefs
///

/// Unique-TObject-ptr collection
typedef std::set<TObject*> EsObjectSet;

/// FWD decls
class ESCORE_GUI_PKG EsI18nManager;
class ESCORE_GUI_PKG EsBusyMessage;
class ESCORE_GUI_PKG TFrmReflectedClassConfig;
class ESCORE_GUI_PKG TFmeBrowseForFolder;
class ESCORE_GUI_PKG TFmeProgressMonitorView;
class ESCORE_GUI_PKG TDlgBase;
class ESCORE_GUI_PKG TDlgUserPrompt;
class ESCORE_GUI_PKG TDlgBrowseForFolder;

// our headers
#if defined(ES_USE_AUTO_UPDATER) && (ES_OS == ES_OS_WINDOWS)
# include <escore-gui/EsAutoUpdater.h>
#endif
#include <escore-gui/EsI18nManager.h>
#include <escore-gui/TDlgBase.h>
#include <escore-gui/TDlgUserPrompt.h>
#include <escore-gui/EsCoreGuiUtilities.h>
#include <escore-gui/EsCoreGui.h>
#include <escore-gui/EsBusyMessage.h>
#include <escore-gui/TFrmReflectedClassConfig.h>
#include <escore-gui/TFmeBrowseForFolder.h>
#include <escore-gui/TFmeProgressMonitorView.h>
#include <escore-gui/TDlgBrowseForFolder.h>

#endif // _es_core_gui_h_
