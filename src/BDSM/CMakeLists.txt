
## ------------------------------------------------------------------------------
## Project description

## Name of the project
PROJECT (BDSM Fortran)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BDSM_SOURCE_DIR}
  ${BDSM_SOURCE_DIR}/..
  ${BDSM_SOURCE_DIR}/../..
  ${BDSM_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (BDSM_BUILD_TESTS       "Build the test programs?"             YES )
option (BDSM_ENABLE_TESTING    "Build the test programs?"             YES )
option (BDSM_VERBOSE_CONFIGURE "Verbose output during configuration?" NO  )

## ------------------------------------------------------------------------------
## Find external libraries

if (NOT BDSM_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY 1)
  set (PGPLOT_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
endif (NOT BDSM_VERBOSE_CONFIGURE)

## --- CFITSIO ---------------------------------

if (NOT HAVE_CFITSIO)
  include (FindCFITSIO)
endif (NOT HAVE_CFITSIO)

## --- PGPLOT ----------------------------------
if (NOT HAVE_PGPLOT)
  include (FindPGPLOT)
endif (NOT HAVE_PGPLOT)

## ------------------------------------------------------------------------------
## Subdirectories of the project

SUBDIRS (implement)
