# top-level cmakefile for esfwx-gui-wx
cmake_minimum_required(VERSION 3.7)
project(esfwx-gui-fmx)

include(esgui.config.cmake)

# esfwx-gui version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/EsCoreGuiConfig.h.in"
  "${ES_PROJECT_ROOT}/EsCoreGuiConfig.h"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
)

include_directories(	
  ${ES_PROJECT_ROOT}
  ${ES_PROJECT_ROOT}/esfwx
  ${ES_PROJECT_ROOT}/esfwx/compatibility-headers						
  ${ES_PROJECT_ROOT}/esfwx/external
)
	
# esfwx-gui components and dependencies
#
# escore-gui
add_subdirectory(escore-gui)

# escore-gui dependencies
add_dependencies(
  ${ESCOREGUI_LIB_TARGET} 
	${ESCORE_LIB_TARGET} 
)

if(ESGUICONFIG_USE_COMMGUI)
# escomm-gui
  add_subdirectory(escomm-gui)
  
  add_dependencies(
    ${ESCOMMGUI_LIB_TARGET} 
    ${ESCORE_LIB_TARGET} 
    ${ESCOMM_LIB_TARGET}
    ${ESCOREGUI_LIB_TARGET}
  )
endif()

if(ESGUICONFIG_USE_CHART)
# eschart
  add_subdirectory(eschart)
  
  add_dependencies(
    ${ESCHART_LIB_TARGET}
    ${ESCORE_LIB_TARGET}  
    ${ESMATH_LIB_TARGET}
  )
endif()

