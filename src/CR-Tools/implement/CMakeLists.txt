##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

## Core modules

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Imaging
  IO
  LopesLegacy
  Math
  Observation
  Utilities
  )

## (optional) support for applications requiring additional external components

if (HAVE_PGPLOT)
  list (APPEND cr_modules Display)
  add_definitions (-DNEED_FORTRAN_UNDERSCORES)
endif (HAVE_PGPLOT)

if (CR_GLISH_SUPPORT)
  list (APPEND cr_modules ApplicationSupport)
endif (CR_GLISH_SUPPORT)

if (HAVE_ROOT AND HAVE_FFTW3)
  list (APPEND cr_modules LopesStar)
endif (HAVE_ROOT AND HAVE_FFTW3)

## register the directories containing the source code

subdirs (${cr_modules})

## -----------------------------------------------------------------------------
## Build "libcr"

## Assemble list of source files

foreach (cr_module ${cr_modules})
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

if (CR_GLISH_SUPPORT AND HAVE_CASA)
  list (APPEND cr_sources templates.cc)
endif (CR_GLISH_SUPPORT AND HAVE_CASA)

## Optional (pre-)compiler settings

if (CR_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (CR_DEBUGGING_MESSAGES)

## Create the library

add_library (cr ${cr_sources})
set_target_properties (cr
  PROPERTIES
  COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC"
  )
target_link_libraries (cr
  ${CASA_LIBRARIES}
  ${G2C_LIBRARIES}
  ${LAPACK_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${X11_LIBRARIES}
  ${libm}
  )

## ------------------------------------------------------------------------------
## Test programs

if (CR_BUILD_TESTS)
  foreach (cr_module ${cr_modules})
    ADD_SUBDIRECTORY ("${cr_module}/test")
  endforeach (cr_module)
endif (CR_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (
  TARGETS cr
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## installation of header files
