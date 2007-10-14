##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 873 2007-10-11 16:16:03Z baehren                        $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Modules inside the "DSP" package

## Core modules

set (dsp_modules
#  Filtering
#  Flagging
  IO
  )

## -----------------------------------------------------------------------------
## Build "libdsp"

## Assemble list of source files

foreach (dsp_module ${dsp_modules})
  ## register the directories containing the source code
  add_subdirectory (${dsp_module})
  ## list of source files for this module
  FILE (GLOB dsp_${dsp_module} ${dsp_module}/*.cpp)
  LIST (APPEND dsp_sources ${dsp_${dsp_module}})
endforeach (dsp_module)

## Optional (pre-)compiler settings

if (DSP_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (DSP_DEBUGGING_MESSAGES)

## Assemble list of external libraries against which to link

if (CASA_LIBRARIES)
  list (APPEND dsp_external_libraries ${CASA_LIBRARIES})
endif (CASA_LIBRARIES)

if (G2C_LIBRARIES)
  list (APPEND dsp_external_libraries ${G2C_LIBRARIES})
endif (G2C_LIBRARIES)

if (LAPACK_LIBRARIES)
  list (APPEND dsp_external_libraries ${LAPACK_LIBRARIES})
endif (LAPACK_LIBRARIES)

if (CFITSIO_LIBRARIES)
  list (APPEND dsp_external_libraries ${CFITSIO_LIBRARIES})
endif (CFITSIO_LIBRARIES)

if (WCSLIB_LIBRARIES)
  list (APPEND dsp_external_libraries ${WCSLIB_LIBRARIES})
endif (WCSLIB_LIBRARIES)

if (X11_LIBRARIES)
  list (APPEND dsp_external_libraries ${X11_LIBRARIES})
endif (X11_LIBRARIES)

if (HAVE_PGPLOT)
  list (APPEND dsp_external_libraries ${PGPLOT_LIBRARIES})
endif (HAVE_PGPLOT)

if (HAVE_PNG)
  list (APPEND dsp_external_libraries ${PNG_LIBRARIES})
endif (HAVE_PNG)

if (libm)
  list (APPEND dsp_external_libraries ${libm})
endif (libm)

## ------------------------------------------------------------------------------
## Create the library

add_library (dsp ${dsp_sources})

target_link_libraries (dsp ${dsp_external_libraries})

## ------------------------------------------------------------------------------
## Test programs

if (DSP_BUILD_TESTS)
  foreach (dsp_module ${dsp_modules})
    ADD_SUBDIRECTORY ("${dsp_module}/test")
  endforeach (dsp_module)
endif (DSP_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (
  TARGETS dsp
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## installation of header files
