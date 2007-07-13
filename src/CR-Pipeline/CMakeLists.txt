##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

project (CR)

## ------------------------------------------------------------------------------
## Subdirectories

subdirs (
  implement
  apps
  test
  )

## ------------------------------------------------------------------------------
## External dependencies; check if we need to search for some components.

## Blitz++

if (NOT HAVE_BLITZ)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindBlitz.cmake)
endif (NOT HAVE_BLITZ)

if (HAVE_BLITZ)
  add_definitions (-DHAVE_BLITZ)
endif (HAVE_BLITZ)

## CASA

if (NOT HAVE_CASA) 
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCASA.cmake)
endif (NOT HAVE_CASA)

if (HAVE_CASA)
  add_definitions (-DHAVE_CASA)
endif (HAVE_CASA)

## CFITSIO

if (NOT HAVE_CFITSIO)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCFITSIO.cmake)
endif (NOT HAVE_CFITSIO)

if (NOT HAVE_FFTW3)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindFFTW3.cmake)
endif (NOT HAVE_FFTW3)

if (NOT HAVE_LAPACK)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindLAPACK.cmake)
endif (NOT HAVE_LAPACK)

if (NOT HAVE_PGPLOT)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindPGPLOT.cmake)
endif (NOT HAVE_PGPLOT)

if (NOT HAVE_ROOT)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindROOT.cmake)
endif (NOT HAVE_ROOT)

if (NOT HAVE_WCS) 
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindWCS.cmake)
endif (NOT HAVE_WCS)

message ("[CR-Pipeline] External library settings summary:")
message ("-- BLITZ   : ${HAVE_BLITZ}   ")
message ("-- CASA    : ${HAVE_CASA}    ")
message ("-- CFITSIO : ${HAVE_CFITSIO} ")
message ("-- FFTW3   : ${HAVE_FFTW3}   ")
message ("-- LAPACK  : ${HAVE_LAPACK}  ")
message ("-- PGPLOT  : ${HAVE_PGPLOT}  ")
message ("-- ROOT    : ${HAVE_ROOT}    ")
message ("-- WCS     : ${HAVE_WCS}     ")

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

## Locate the installation area

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

