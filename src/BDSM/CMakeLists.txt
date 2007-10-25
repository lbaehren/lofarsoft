
## ------------------------------------------------------------------------------
## Project description

PROJECT (BDSM Fortran)

## ------------------------------------------------------------------------------
## Find external libraries

## check where to find the CMake scripts

find_path (bdsm_cmake FindCFITSIO.cmake FindPGPLOT.cmake
  PATHS 
  ${BDSM_SOURCE_DIR}
  ${BDSM_SOURCE_DIR}/..
  ${BDSM_SOURCE_DIR}/../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (bdsm_cmake)
  ## --- CFITSIO ---------------------------------
  if (NOT HAVE_CFITSIO)
    include (${bdsm_cmake}/FindCFITSIO.cmake)
  endif (NOT HAVE_CFITSIO)
  ## --- PGPLOT ----------------------------------
  if (NOT HAVE_PGPLOT)
    include (${bdsm_cmake}/FindPGPLOT.cmake)
  endif (NOT HAVE_PGPLOT)
endif (bdsm_cmake)

SUBDIRS (implement)
