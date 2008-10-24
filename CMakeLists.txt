
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1620 2008-06-06 09:26:35Z baehren                       $
##-------------------------------------------------------------------------------


## Name of the project handled by CMake
project (LUS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${LUS_SOURCE_DIR}
  ${LUS_SOURCE_DIR}/..
  ${LUS_SOURCE_DIR}/../..
  ${LUS_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ==============================================================================
##
##  List of selectable packages/libraries
##
## ==============================================================================

## External packages

option (BUILD_BISON     "Build Bison?"                   YES )
option (BUILD_BOOST     "Build the Boost libraries?"     YES )
option (BUILD_CASACORE  "Build casacore?"                YES )
option (BUILD_CFITSIO   "Build the CFITSIO library?"     YES )
option (BUILD_MATHGL    "Build the MathGL package?"      YES )
option (BUILD_STARTOOLS "Build the Star-Tools package?"  YES )
option (BUILD_WCSLIB    "Build the WCS library?"         YES )
option (BUILD_WCSTOOLS  "Build the WCS-Tools package?"   YES )

## USG packages

option (BUILD_DAL      "Build the Data Access Library (DAL)?"     NO )
option (BUILD_CONTRIB  "Build contributed programs and scripts?"  NO )
option (BUILD_CR       "Build the Cosmic Ray Tools?"              NO )

## ==============================================================================
##
##  External packages
##
## ==============================================================================

## ------------------------------------------------------------------------------
## CASACORE

if (BUILD_CASACORE)
  add_subdirectory (${LUS_SOURCE_DIR}/external/wcslib)
  add_subdirectory (${LUS_SOURCE_DIR}/external/cfitsio)
  add_subdirectory (${LUS_SOURCE_DIR}/external/casacore)
endif (BUILD_CASACORE)

## ------------------------------------------------------------------------------
## CFITSIO

if (BUILD_CFITSIO)
  add_subdirectory (${LUS_SOURCE_DIR}/external/cfitsio)
endif (BUILD_CFITSIO)

## ------------------------------------------------------------------------------
## PLPLOT

if (BUILD_PLPLOT)
  add_subdirectory (${LUS_SOURCE_DIR}/external/plplot)
endif (BUILD_PLPLOT)

## ------------------------------------------------------------------------------
## STARTOOLS

if (BUILD_STARTOOLS)
  add_subdirectory (${LUS_SOURCE_DIR}/external/startools)
endif (BUILD_STARTOOLS)

## ------------------------------------------------------------------------------
## WCSLIB

if (BUILD_WCSLIB)
  add_subdirectory (${LUS_SOURCE_DIR}/external/wcslib)
endif (BUILD_WCSLIB)

## ------------------------------------------------------------------------------
## WCSTOOLS

if (BUILD_WCSTOOLS)
  add_subdirectory (${LUS_SOURCE_DIR}/external/wcstools)
endif (BUILD_WCSTOOLS)
