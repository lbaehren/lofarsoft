
## ------------------------------------------------------------------------------
## Project description

## Name of the project
project (BDSM Fortran C)

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

option (BDSM_BUILD_TESTS       "Build the test programs?"              YES )
option (BDSM_ENABLE_TESTING    "Build the test programs?"              YES )
option (BDSM_VERBOSE_CONFIGURE "Verbose output during configuration?"  NO  )
option (BDSM_WITH_FBDSM        "Build Fortran-based version of BDSM?"  YES )
option (BDSM_WITH_PYBDSM       "Build Python-enabled version of BDSM?" YES )

## ==============================================================================
##
##  External dependencies
##
## ==============================================================================

include (IncludeFindModule)

set (bdsm_cmake_modules
  Boost
  CFITSIO
  NumPy
  NumUtil
  GFortran
  PGPLOT
  Python
  WCSLIB
  )

include_find_module (${bdsm_cmake_modules} ${BDSM_VERBOSE_CONFIGURE})

## ==============================================================================
##
##  Handle options and external dependencies
##
## ==============================================================================

if (NOT HAVE_BOOST OR NOT BOOST_PYTHON_LIBRARY)
  message (STATUS "[BDSM] Unable to build pybdsm - missing Boost!")
  set (BDSM_WITH_PYBDSM FALSE)
endif (NOT HAVE_BOOST OR NOT BOOST_PYTHON_LIBRARY)

if (NOT HAVE_NUMPY)
  message (STATUS "[BDSM] Unable to build pybdsm - missing NumPy!")
  set (BDSM_WITH_PYBDSM FALSE)
endif (NOT HAVE_NUMPY)

if (NOT HAVE_PYTHON)
  message (STATUS "[BDSM] Unable to build pybdsm - missing Python!")
  set (BDSM_WITH_PYBDSM FALSE)
endif (NOT HAVE_PYTHON)

## ==============================================================================
##
##  Project subdirectories
##
## ==============================================================================

add_subdirectory (implement)
add_subdirectory (scripts)
