#ifndef _es_comm_gui_h_
#define _es_comm_gui_h_

#ifndef _es_core_gui_h_
#	error File escore-gui.h must be included prior to this header!
#endif

// escore export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESCOMMGUI_EXPORTS
#		define ESCOMM_GUI_CLASS	      ES_EXPORT_SPEC
#		define ESCOMM_GUI_FUNC(type)	ES_FUNCEXPORT_SPEC(type)
#		define ESCOMM_GUI_DATA(type)	ES_DATAEXPORT_SPEC(type)
#		define ESCOMM_GUI_PKG		      PACKAGE
#	else
#		define ESCOMM_GUI_CLASS	      ES_IMPORT_SPEC
#		define ESCOMM_GUI_FUNC(type)	ES_FUNCIMPORT_SPEC(type)
#		define ESCOMM_GUI_DATA(type)	ES_DATAIMPORT_SPEC(type)
#		define ESCOMM_GUI_PKG		      PACKAGE
#	endif
#else
#	define ESCOMM_GUI_CLASS
#	define ESCOMM_GUI_FUNC(type)   type
#	define ESCOMM_GUI_DATA(type)   type
#	define ESCOMM_GUI_PKG          PACKAGE
#endif

#include <escomm-gui/TFrmChannelIoConfig.h>
#include <escomm-gui/TFrmConnectionConfig.h>
#include <escomm-gui/TDlgChannelIoConfig.h>
#include <escomm-gui/TDlgConnectionConfig.h>

#endif // _es_comm_gui_h_
