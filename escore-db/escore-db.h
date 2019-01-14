#ifndef _es_core_db_h_
#define _es_core_db_h_

#ifndef _es_core_h_
#	error escore.h header file must be included prior to this header!
#endif

#ifndef _es_core_gui_h_
#	error File escore-gui.h must be included prior to this header!
#endif

// escore export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCOREDB_EXPORTS
#		define ESCORE_DB_CLASS	    ES_EXPORT_SPEC
#		define ESCORE_DB_FUNC(type)	ES_FUNCEXPORT_SPEC(type)
#		define ESCORE_DB_DATA(type)	ES_DATAEXPORT_SPEC(type)
#		define ESCORE_DB_PKG		    PACKAGE
#	else
#		define ESCORE_DB_CLASS	    ES_IMPORT_SPEC
#		define ESCORE_DB_FUNC(type)	ES_FUNCIMPORT_SPEC(type)
#		define ESCORE_DB_DATA(type)	ES_DATAIMPORT_SPEC(type)
#		define ESCORE_DB_PKG		    PACKAGE
#	endif
#else
#	define ESCORE_DB_CLASS
#	define ESCORE_DB_FUNC(type)   type
#	define ESCORE_DB_DATA(type)   type
#	define ESCORE_DB_PKG
#endif

// system|rtl stuff
#include <System.hpp>
#include <System.Rtti.hpp>
#include <System.Classes.hpp>
#include <Data.DB.hpp>
#include <fmx.h>

/// Common typedefs
///

// our headers
#include <escore-db/EsCoreDbUtilities.h>

#endif // _es_core_db_h_
