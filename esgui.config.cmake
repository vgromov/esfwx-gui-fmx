# frameworkGui version definitions
set(frameworkGuiVendor			"ECO-Electronics" CACHE INTERNAL "")
set(frameworkGuiVendorShort "ECO-E" CACHE INTERNAL "")
set(frameworkGui						"ECO-E GUI framework" CACHE INTERNAL "")
set(frameworkGuiCopyright	  "Copyright 2009-2022" CACHE INTERNAL "")
set(frameworkGui_VER_MAJOR	1 CACHE INTERNAL "")
set(frameworkGui_VER_MINOR	24 CACHE INTERNAL "")
set(frameworkGui_BUILD_NUM	4 CACHE INTERNAL "")

if(WIN32)
  set(ESGUICONFIG_USE_AUTOUPDATE 1 CACHE BOOL "Build-in online auto update suppot class. Default=ON")
else()
  set(ESGUICONFIG_USE_AUTOUPDATE 0 CACHE INTERNAL "")
endif()

set(ESGUICONFIG_USE_COMMGUI 0 CACHE BOOL "Build COMM GUI shared classes. Default=OFF")
#set(ESGUICONFIG_USE_CHART 0 CACHE BOOL "Build CHART shared classes. Default=OFF")

set(ESGUI_VERSION_SUFFIX  "v${frameworkGui_VER_MAJOR}_${frameworkGui_VER_MINOR}_${frameworkGui_BUILD_NUM}" CACHE INTERNAL "")
message(
  STATUS
  "ESGUI_VERSION_SUFFIX=>${ESGUI_VERSION_SUFFIX}"
)
