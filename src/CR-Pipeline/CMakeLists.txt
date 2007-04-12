
project (CR)

## ------------------------------------------------------------------------------
## External dependencies; check if we need to search for some components.

if (NOT HAVE_BLITZ)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindBlitz.cmake)
endif (NOT HAVE_BLITZ)

if (NOT HAVE_CASA) 
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCASA.cmake)
endif (NOT HAVE_CASA)

if (NOT HAVE_CFITSIO)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCFITSIO.cmake)
endif (NOT HAVE_CFITSIO)

if (NOT HAVE_FFTW3)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindFFTW3.cmake)
endif (NOT HAVE_FFTW3)

if (NOT HAVE_LAPACK)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindLAPACK.cmake)
endif (NOT HAVE_LAPACK)

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
message ("-- ROOT    : ${HAVE_ROOT}    ")
message ("-- WCS     : ${HAVE_WCS}     ")

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  .
  ..
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/test
  ${BLITZ_INCLUDES}
  ${CASA_INCLUDES}
  ${CFITSIO_INCLUDES}
  ${ROOT_INCLUDES}
  ${WCS_INCLUDES}
  )

## ------------------------------------------------------------------------------
## Subdirectories

subdirs (
  apps
  implement
  test
  )

## ------------------------------------------------------------------------------
## Verbose level

SET (CMAKE_VERBOSE_MAKEFILE 1)
