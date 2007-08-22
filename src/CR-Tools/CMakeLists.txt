
##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## Name of the project
project (CR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## User defined build/install options

option (CR_BUILD_TESTS "Build the test programs?" 1)
option (CR_ENABLE_TESTING "Build the test programs?" 1)
option (CR_DEBUGGING_MESSAGES "Print debugging information?" 0)
option (CR_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)
option (CR_GLISH_SUPPORT "Activate support for Glish applications?" 1)
option (CR_PYTHON_SUPPORT "Activate support for Glish applications?" 0)

## Handle option: testing mode on/off

if (CR_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (CR_ENABLE_TESTING)

## Handle option: verbose mode on/off

if (NOT CR_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY 1)
  set (CASA_FIND_QUIETLY 1)
  set (CASACORE_FIND_QUIETLY 1)
  set (CFITSIO_FIND_QUIETLY 1)
  set (FFTW3_FIND_QUIETLY 1)
  set (GLISH_FIND_QUIETLY 1)
  set (LAPACK_FIND_QUIETLY 1)
  set (PGPLOT_FIND_QUIETLY 1)
  set (ROOT_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
endif (NOT CR_VERBOSE_CONFIGURE)

## -----------------------------------------------------------------------------
## Subdirectories

subdirs (
  implement
  apps
  )

if (CR_BUILD_TESTS)
  subdirs (test)
endif (CR_BUILD_TESTS)

## -----------------------------------------------------------------------------
## External dependencies; check if we need to search for some components.

## check where to find the CMake scripts

find_path (cr_cmake FindBlitz.cmake FindCASA.cmake FindCFITSIO.cmake
  PATHS 
  ${CR_SOURCE_DIR}
  ${CR_SOURCE_DIR}/..
  ${CR_SOURCE_DIR}/../..
  ${CR_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## check for the external packages

find_package (Motif)
find_package (PNG)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

if (cr_cmake)
  ## Blitz++
  if (NOT HAVE_BLITZ)
    include (${cr_cmake}/FindBlitz.cmake)
  endif (NOT HAVE_BLITZ)
  if (HAVE_BLITZ)
    add_definitions (-DHAVE_BLITZ)
  endif (HAVE_BLITZ)
  ## CASA - search for full installation first
  if (CR_GLISH_SUPPORT)
    if (NOT HAVE_CASA)
      message (STATUS "[CR] Configuration to use CASA ...")
      include (${cr_cmake}/FindCASA.cmake)
    endif (NOT HAVE_CASA)
  endif (CR_GLISH_SUPPORT)
  if (HAVE_CASA)
    add_definitions (-DHAVE_CASA)
  else (HAVE_CASA)
    message (STATUS "[CR] Failed to configure using CASA !")
  endif (HAVE_CASA)
  ## casacore - actually for the library that is all we need
  if (NOT HAVE_CASA) 
    message (STATUS "[CR] Configuration to use CASACORE ...")
    include (${cr_cmake}/FindCASACORE.cmake)
  endif (NOT HAVE_CASA)
  if (HAVE_CASACORE)
    add_definitions (-DHAVE_CASA)
    add_definitions (-DHAVE_CASACORE)
  else (HAVE_CASACORE)
    set (HAVE_CASACORE FALSE)
  endif (HAVE_CASACORE)
  ## CFITSIO
  if (NOT HAVE_CFITSIO)
    include (${cr_cmake}/FindCFITSIO.cmake)
  endif (NOT HAVE_CFITSIO)
  ## FFTW3
  if (NOT HAVE_FFTW3)
    include (${cr_cmake}/FindFFTW3.cmake)
  endif (NOT HAVE_FFTW3)
  ## LAPACK
  if (NOT HAVE_LAPACK)
    include (${cr_cmake}/FindLAPACK.cmake)
  endif (NOT HAVE_LAPACK)
  ## -- PGPlot ---------------
  if (NOT HAVE_PGPLOT)
    include (${cr_cmake}/FindPGPLOT.cmake)
  endif (NOT HAVE_PGPLOT)
  ## -- popt -----------------
  if (NOT HAVE_POPT)
    include (${cr_cmake}/FindPOPT.cmake)
  endif (NOT HAVE_POPT)
  ## -- ROOT -----------------
  if (NOT HAVE_ROOT)
    include (${cr_cmake}/FindROOT.cmake)
  endif (NOT HAVE_ROOT)
  ## -- WCSLIB ---------------
  if (NOT HAVE_WCSLIB) 
    include (${cr_cmake}/FindWCS.cmake)
  endif (NOT HAVE_WCSLIB)
elseif (cr_cmake)
  message (SEND_ERROR "Unable to locate additional CMake scripts!")
endif (cr_cmake)

## ------------------------------------------------------------------------------
## Check for the presence of further USG comonents

find_path (DAL_INCLUDES dal.h lopesevent.h
  PATHS ${CR_SOURCE_DIR}/.. ..
  PATH_SUFFIXES DAL/implement
  )

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/apps
  ${CR_SOURCE_DIR}/test
  ${DAL_INCLUDES}
  ${BLITZ_INCLUDES}
  ${CASA_INCLUDES}
  ${CFITSIO_INCLUDES}
  ${PGPLOT_INCLUDES}
  ${ROOT_INCLUDES}
  ${WCSLIB_INCLUDES}
  )

## ------------------------------------------------------------------------------
## Verbose level

SET (CMAKE_VERBOSE_MAKEFILE 0)

## ------------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${CR_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "[CR] Installation area located.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## ------------------------------------------------------------------------------
## Configuration summary

message (STATUS "CMAKE_SYSTEM .......... : ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR  : ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_SYSTEM_64BIT .....: ${CMAKE_SYSTEM_64BIT}")
message (STATUS "CMAKE_SYSTEM_BIG_ENDIAN : ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS "BLITZ ................. : ${HAVE_BLITZ}")
message (STATUS "CASA .................. : ${HAVE_CASA}")
message (STATUS "CASACORE .............. : ${HAVE_CASACORE}")
message (STATUS "CFITSIO ............... : ${HAVE_CFITSIO}")
message (STATUS "FFTW3 ................. : ${HAVE_FFTW3}")
message (STATUS "LAPACK ................ : ${HAVE_LAPACK}")
message (STATUS "PGPLOT ................ : ${HAVE_PGPLOT}")
message (STATUS "ROOT .................. : ${HAVE_ROOT}")
message (STATUS "WCSLIB ................ : ${HAVE_WCSLIB}")
