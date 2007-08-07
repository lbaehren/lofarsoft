##-------------------------------------------------------------------------------
## $Id::                                                                        $
##-------------------------------------------------------------------------------

## Name of the project
project (CR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## User defined build/install options

option (CR_BUILD_TESTS "Build the test programs?" 1)
option (CR_DEBUGGING_MESSAGES "Print debugging information?" 0)

## ------------------------------------------------------------------------------
## Subdirectories

subdirs (
  implement
#  apps
  )

if (CR_BUILD_TESTS)
  subdirs (test)
endif (CR_BUILD_TESTS)

## ------------------------------------------------------------------------------
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

if (cr_cmake)
  ## Blitz++
  if (NOT HAVE_BLITZ)
    include (${cr_cmake}/FindBlitz.cmake)
  endif (NOT HAVE_BLITZ)
  if (HAVE_BLITZ)
    add_definitions (-DHAVE_BLITZ)
  endif (HAVE_BLITZ)
  ## CASA
  if (NOT HAVE_CASA) 
    include (${cr_cmake}/FindCASA.cmake)
  endif (NOT HAVE_CASA)
  if (HAVE_CASA)
    add_definitions (-DHAVE_CASA)
  endif (HAVE_CASA)
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
  ## -- ROOT -----------------
  if (NOT HAVE_ROOT)
    include (${cr_cmake}/FindROOT.cmake)
  endif (NOT HAVE_ROOT)
  ## -- WCSLIB ---------------
  if (NOT HAVE_WCS) 
    include (${cr_cmake}/FindWCS.cmake)
  endif (NOT HAVE_WCS)
elseif (cr_cmake)
  message (SEND_ERROR "Unable to locate additional CMake scripts!")
endif (cr_cmake)


message (STATUS "[CR-Pipeline] External library settings summary:")
message (STATUS "  BLITZ   : ${HAVE_BLITZ}   ")
message (STATUS "  CASA    : ${HAVE_CASA}    ")
message (STATUS "  CFITSIO : ${HAVE_CFITSIO} ")
message (STATUS "  FFTW3   : ${HAVE_FFTW3}   ")
message (STATUS "  LAPACK  : ${HAVE_LAPACK}  ")
message (STATUS "  PGPLOT  : ${HAVE_PGPLOT}  ")
message (STATUS "  ROOT    : ${HAVE_ROOT}    ")
message (STATUS "  WCS     : ${HAVE_WCS}     ")

## ------------------------------------------------------------------------------
## Check for the presence of further USG comonents

find_path (DAL_INCLUDES dal.h lopesevent.h
  PATHS ${CR_SOURCE_DIR}/.. ..
  PATH_SUFFIXES DAL/implement
  )

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  .
  ..
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/test
  ${DAL_INCLUDES}
  ${BLITZ_INCLUDES}
  ${CASA_INCLUDES}
  ${CFITSIO_INCLUDES}
  ${PGPLOT_INCLUDES}
  ${ROOT_INCLUDES}
  ${WCS_INCLUDES}
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
  message (STATUS "Installation area located for package CR.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

