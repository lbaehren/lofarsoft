##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Display
  Imaging
  IO
  LopesBase
  Math
  Observation
  Skymap
  Utilities
  )

subdirs (${cr_modules})

## -----------------------------------------------------------------------------
## collect the source files in the modules

foreach (cr_module ${cr_modules})
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## -----------------------------------------------------------------------------
## build the library

if (CR_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (CR_DEBUGGING_MESSAGES)

if (CASA_LIBRARIES AND LAPACK_LIBRARIES)
  add_library (cr
    ${cr_sources}
    templates.h
    )
  target_link_libraries (cr
    ${CASA_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${CPGPLOT_LIBRARIES}
    )
else (CASA_LIBRARIES AND LAPACK_LIBRARIES)
  message (STATUS "Sorry, unable to build CR library; missing CASA and LAPACK!")
endif (CASA_LIBRARIES AND LAPACK_LIBRARIES)

## ------------------------------------------------------------------------------
## Test programs

if (CR_BUILD_TESTS)
  ## build up the list of directories ...
  foreach (cr_module ${cr_modules})
    list (APPEND tests "${cr_module}/test")
  endforeach (cr_module)
  ## ... and register them
  subdirs (${tests})
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
