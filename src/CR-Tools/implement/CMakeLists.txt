##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

## Core modules which should be included in any case

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

## optional modules (added depending on requested features and installed packages)

# (a) Display modules requires at least one of the external plotting libraries to
#     be available

if (HAVE_PGPLOT OR HAVE_PLPLOT)
  list (APPEND cr_modules Display)
  add_definitions (-DNEED_FORTRAN_UNDERSCORES)
endif (HAVE_PGPLOT OR HAVE_PLPLOT)

# (b) Support for usage of Glish objects and communication bus

if (CR_GLISH_SUPPORT)
  list (APPEND cr_modules ApplicationSupport)
endif (CR_GLISH_SUPPORT)

# (c) Data processing routines written for the LOPES-Star experiment

if (CR_ENABLE_LOPESSTAR)
  list (APPEND cr_modules LopesStar)
endif (CR_ENABLE_LOPESSTAR)

## -----------------------------------------------------------------------------
## Build "libcr"

## [1] Assemble list of source files

foreach (cr_module ${cr_modules})
  ## list of source files for this module
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

if (CR_GLISH_SUPPORT AND HAVE_CASA)
  list (APPEND cr_sources templates.cc)
endif (CR_GLISH_SUPPORT AND HAVE_CASA)

if (NOT HAVE_PGPLOT)
  list (REMOVE_ITEM cr_sources Display/PGPlotterCR.cc)
  list (REMOVE_ITEM cr_sources Display/PGPlotterLocal.cc)
endif (NOT HAVE_PGPLOT)

if (NOT CR_ENABLE_LOPESSTAR)
  list (REMOVE_ITEM cr_sources Analysis/CompletePipeline.cc)
  list (REMOVE_ITEM cr_sources Analysis/analyseLOPESevent2.cc)
endif (NOT CR_ENABLE_LOPESSTAR)

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

if (PGPLOT_LIBRARIES)
  list (APPEND cr_link_libraries ${PGPLOT_LIBRARIES})
endif (PGPLOT_LIBRARIES)

if (PLPLOT_LIBRARIES)
  list (APPEND cr_link_libraries ${PLPLOT_LIBRARIES})
endif (PLPLOT_LIBRARIES)

if (FFTW3_LIBRARIES)
  list (APPEND cr_link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (ROOT_LIBRARIES)
  list (APPEND cr_link_libraries ${ROOT_LIBRARIES})
endif (ROOT_LIBRARIES)

if (TCL_LIBRARIES)
  list (APPEND cr_link_libraries ${TCL_LIBRARIES})
endif (TCL_LIBRARIES)

if (TK_LIBRARIES)
  list (APPEND cr_link_libraries ${TK_LIBRARIES})
endif (TK_LIBRARIES)

if (PostgreSQL_LIBRARIES)
  list (APPEND cr_link_libraries ${PostgreSQL_LIBRARIES})
endif (PostgreSQL_LIBRARIES)

if (PNG_LIBRARIES)
  list (APPEND cr_link_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (GIF_LIBRARIES)
  list (APPEND cr_link_libraries ${GIF_LIBRARIES})
endif (GIF_LIBRARIES)

if (libgd)
  list (APPEND cr_link_libraries ${libgd})
endif (libgd)

if (libutil)
  list (APPEND cr_link_libraries ${libutil})
endif (libutil)

if (libm)
  list (APPEND cr_link_libraries ${libm})
endif (libm)

## ------------------------------------------------------------------------------
## Create the library

include_directories (
  ${CR_SOURCE_DIR}/implement
  ${CASA_INCLUDES}
  )

add_definitions (
  -fPIC
  -ftemplate-depth-100
  )

add_library (cr ${cr_sources})

if (NOT APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC"
    LINK_FLAGS "-bind_at_load"
    )
endif (NOT APPLE)

target_link_libraries (cr
  ${cr_link_libraries}
  )

## ------------------------------------------------------------------------------
## Create Python bindings for the library

if (CR_PYTHON_BINDINGS)
  
  ## compiler instructions
  add_library (pycr MODULE
    ${cr_sources}
    BindingsBoostPython.cc
    )
  ## linker instructions
  target_link_libraries(pycr
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${cr_link_libraries}
    )
  ## additional target properties
  if (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON -fpermissive"
      LINK_FLAGS "-fPIC -flat_namespace"
      )
  else (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS -DPYTHON
      LINK_FLAGS "-fPIC -shared"
      )
  endif (APPLE)
  ## installation
  install (
    TARGETS pycr
    LIBRARY DESTINATION lib/python
    )
  
endif (CR_PYTHON_BINDINGS)

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
