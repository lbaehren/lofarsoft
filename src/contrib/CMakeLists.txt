##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 494 2007-08-02 09:39:15Z baehren                       $
##------------------------------------------------------------------------------

## Name of the project
project (contrib)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Options

option (contrib_VERBOSE_CONFIGURE "Verbose output during configuration?"     NO )
option (contrib_ENABLE_TESTING    "Build and enable running test programs?"  YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${contrib_SOURCE_DIR}
  ${contrib_SOURCE_DIR}/..
  ${contrib_SOURCE_DIR}/../..
  ${contrib_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## -----------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${contrib_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for user contributions.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## ------------------------------------------------------------------------------
## Locate components of required external packages

if (NOT contrib_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY      YES )
  set (BOOST_FIND_QUIETLY      YES )
  set (CASA_FIND_QUIETLY       YES )
  set (CASACORE_FIND_QUIETLY   YES )
  set (CFITSIO_FIND_QUIETLY    YES )
  set (DAL_FIND_QUIETLY        YES )
  set (FFTW3_FIND_QUIETLY      YES )
  set (GFORTRAN_FIND_QUIETLY   YES )
  set (GSL_FIND_QUIETLY        YES )
  set (HDF5_FIND_QUIETLY       YES )
  set (LAPACK_FIND_QUIETLY     YES )
  set (PGPLOT_FIND_QUIETLY     YES )
  set (PLPLOT_FIND_QUIETLY     YES )
  set (PNG_FIND_QUIETLY        YES )
  set (POPT_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY     YES )
  set (Python_FIND_QUIETLY     YES )
  set (PostgreSQL_FIND_QUIETLY YES )
  set (QT4_FIND_QUIETLY        YES )
  set (ROOT_FIND_QUIETLY       YES )
  set (STARTOOLS_FIND_QUIETLY  YES )
  set (WCSLIB_FIND_QUIETLY     YES )
  set (X11_FIND_QUIETLY        YES )
  set (ZLIB_FIND_QUIETLY       YES )
endif (NOT contrib_VERBOSE_CONFIGURE)

## Standard CMake modules

if (contrib_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (contrib_ENABLE_TESTING)

find_package (X11)
find_package (ZLIB)

## USG CMake modules 

include (FindBoost)
include (FindCASACORE)
include (FindCFITSIO)
include (FindGFortran)
include (FindHDF5)
include (FindLAPACK)
include (FindPLplot)
include (FindWCS)

## -----------------------------------------------------------------------------
## Configuration summary

if (contrib_VERBOSE_CONFIGURE)
  message (STATUS "[config] Configuration summary.")
  message (STATUS "Have BOOST    = ${HAVE_BOOST}")
  message (STATUS "Have casacore = ${HAVE_CASACORE}")
  message (STATUS "Have CFITSIO  = ${HAVE_CFITSIO}")
  message (STATUS "Have HDF5     = ${HAVE_HDF5}")
  message (STATUS "Have PLPLOT   = ${HAVE_PLPLOT}")
  message (STATUS "Have WCS      = ${HAVE_WCSLIB}")
  message (STATUS "Have ZLIB     = ${HAVE_ZLIB}")
endif (contrib_VERBOSE_CONFIGURE)

## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (data_inspection)
add_subdirectory (testing)
add_subdirectory (utilities)
