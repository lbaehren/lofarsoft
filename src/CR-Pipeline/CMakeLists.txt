
project (CR)

## ------------------------------------------------------------------------------
## External dependencies

if (NOT HAVE_BLITZ)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindBlitz.cmake)
endif (NOT HAVE_BLITZ)

if (NOT HAVE_CASA) 
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCASA.cmake)
endif (NOT HAVE_CASA)

if (NOT HAVE_CFITSIO)
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCFITSIO.cmake)
endif (NOT HAVE_CFITSIO)

include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindFFTW3.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindLAPACK.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindROOT.cmake)

if (NOT HAVE_WCS) 
  include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindWCS.cmake)
endif (NOT HAVE_WCS)

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  .
  ..
  ./implement
  ./test
  ${BLITZ_INCLUDES}
  ${ROOT_INCLUDES}
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

SET (CMAKE_VERBOSE_MAKEFILE 0)
