
## ------------------------------------------------------------------------------
## Project description

## Name of the project
project (BDSM Fortran C CXX)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

if (LUS_SOURCE_DIR)
  include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
else (LUS_SOURCE_DIR)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_SOURCE_DIR)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (ANAAMIKA_BUILD_TESTS       "Build the test programs?"               YES )
option (ANAAMIKA_ENABLE_TESTING    "Build the test programs?"               YES )
option (ANAAMIKA_VERBOSE_CONFIGURE "Verbose output during configuration?"   NO  )
option (ANAAMIKA_ENABLE_FBDSM      "Build Fortran-based version of BDSM?"   NO  )
option (ANAAMIKA_ENABLE_PYBDSM     "Build Python-enabled version of BDSM?"  YES )

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
  X11
  )

include_find_module (${bdsm_cmake_modules} ${ANAAMIKA_VERBOSE_CONFIGURE})

## ==============================================================================
##
##  Handle options and external dependencies
##
## ==============================================================================

if (NOT HAVE_BOOST OR NOT BOOST_PYTHON_LIBRARY)
  message (STATUS "[BDSM] Unable to build pybdsm - missing Boost!")
  set (ANAAMIKA_ENABLE_PYBDSM FALSE)
endif (NOT HAVE_BOOST OR NOT BOOST_PYTHON_LIBRARY)

if (NOT HAVE_NUMPY)
  message (STATUS "[BDSM] Unable to build pybdsm - missing NumPy!")
  set (ANAAMIKA_ENABLE_PYBDSM FALSE)
endif (NOT HAVE_NUMPY)

if (NOT HAVE_PYTHON)
  message (STATUS "[BDSM] Unable to build pybdsm - missing Python!")
  set (ANAAMIKA_ENABLE_PYBDSM FALSE)
endif (NOT HAVE_PYTHON)

if (ANAAMIKA_ENABLE_TESTING)
  include (CTest)
  enable_testing() 
endif (ANAAMIKA_ENABLE_TESTING)

# Set Fortran include directories and compile flags.
include_directories (${CMAKE_SOURCE_DIR}/implement/fBDSM/includes)
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fPIC -fno-second-underscore")

## ==============================================================================
##
##  Project subdirectories
##
## ==============================================================================

add_subdirectory (implement)
add_subdirectory (apps)
