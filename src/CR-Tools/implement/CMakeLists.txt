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

## -----------------------------------------------------------------------------
## Build "libcr"

## Assemble list of source files

foreach (cr_module ${cr_modules})
  ## list of source files for this module
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

## Assemble list of external libraries against which to link

if (CASA_LIBRARIES)
  list (APPEND cr_link_libraries ${CASA_LIBRARIES})
endif (CASA_LIBRARIES)

if (G2C_LIBRARIES)
  list (APPEND cr_link_libraries ${G2C_LIBRARIES})
endif (G2C_LIBRARIES)

if (LAPACK_LIBRARIES)
  list (APPEND cr_link_libraries ${LAPACK_LIBRARIES})
endif (LAPACK_LIBRARIES)

if (CFITSIO_LIBRARIES)
  list (APPEND cr_link_libraries ${CFITSIO_LIBRARIES})
endif (CFITSIO_LIBRARIES)

if (WCSLIB_LIBRARIES)
  list (APPEND cr_link_libraries ${WCSLIB_LIBRARIES})
endif (WCSLIB_LIBRARIES)

if (X11_LIBRARIES)
  list (APPEND cr_link_libraries ${X11_LIBRARIES})
endif (X11_LIBRARIES)

if (HAVE_PGPLOT)
  list (APPEND cr_link_libraries ${PGPLOT_LIBRARIES})
endif (HAVE_PGPLOT)

if (HAVE_PNG)
  list (APPEND cr_link_libraries ${PNG_LIBRARIES})
endif (HAVE_PNG)

if (libm)
  list (APPEND cr_link_libraries ${libm})
endif (libm)

if (libutil)
  list (APPEND cr_link_libraries ${libutil})
endif (libutil)

## ------------------------------------------------------------------------------
## Create the library

add_definitions (
  -fPIC
  -ftemplate-depth-100
  )

add_library (cr ${cr_sources})

if (NOT APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC"
    )
endif (NOT APPLE)

target_link_libraries (cr ${cr_link_libraries})

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
