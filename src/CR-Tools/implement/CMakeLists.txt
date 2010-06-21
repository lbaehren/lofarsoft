##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## =============================================================================
##
##  `libcr` library
##
## =============================================================================

##__________________________________________________________
## Core modules

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Filters
  Imaging
  IO
  Math
  Utilities
  )

##__________________________________________________________
## Optional modules depending on user selection

if (CR_WITH_GLISH)
  list (APPEND cr_modules ApplicationSupport)
  ## additional sources-files
  if (HAVE_CASA)
    list (APPEND cr_sources templates.cc)
  endif (HAVE_CASA)
endif (CR_WITH_GLISH)

if (HAVE_FFTW3)
  list (APPEND cr_modules Analysis/Utils)
endif (HAVE_FFTW3)

##__________________________________________________________
## Collect source files to be compiled into the library

foreach (cr_module ${cr_modules})

  ## list of source files for this module
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc ${cr_module}/*.tcc)
  ## list of header files for this module
  FILE (GLOB cr_${cr_module}_headers ${cr_module}/*.h)

  ## add to global source files list
  LIST (APPEND cr_sources ${cr_${cr_module}})

  ## Installation of header files
  install (FILES ${cr_${cr_module}_headers}
    DESTINATION include/crtools/${cr_module}
    )
endforeach (cr_module)

## ------------------------------------------------------------------------------
## Optional modules (added depending on requested features and installed packages)

## Enable using routines from LOPES-Star?

if (CR_WITH_STARTOOLS)
  LIST (APPEND cr_sources ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C)
else (CR_WITH_STARTOOLS)
  message (STATUS "[CR-Tools] Removing source files requiring STAR-Tools installation.")
  foreach (cr_startools
      CompletePipeline.cc
      analyseLOPESevent2.cc
      checkNoiseInfluence.cc
      PulseProperties.cc
      lateralDistribution.cc
      )
    list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/${cr_startools})
  endforeach (cr_startools)
endif (CR_WITH_STARTOOLS)

if (NOT CR_WITH_PLOTTING)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Utilities/SimplePlot.cc)
endif (NOT CR_WITH_PLOTTING)

## ------------------------------------------------------------------------------
## Create the library

add_library (cr
  ${CR_BINARY_DIR}/crtools.h
  ${cr_sources}
  )

if (APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "-fPIC -ftemplate-depth-100"
    LINK_FLAGS "-bind_at_load"
    )
else (APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC -ftemplate-depth-100"
    LINK_FLAGS "-bind_at_load"
    )
endif (APPLE)

## Linker instructions

target_link_libraries (cr
  ${cr_link_libraries}
  )

## Installation

install (
  TARGETS cr
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## =============================================================================
##
##  Optional libraries/bindings
##
## =============================================================================

add_subdirectory (Bindings)

if (CR_WITH_PYPELINE AND HAVE_FFTW3)
  add_subdirectory (Pypeline)
endif (CR_WITH_PYPELINE AND HAVE_FFTW3)

if (CR_WITH_GUI)
  add_subdirectory (GUI)
endif (CR_WITH_GUI)

## =============================================================================
##
##  Test programs
##
## =============================================================================

if (CR_BUILD_TESTS)
  foreach (cr_module ${cr_modules})
    add_subdirectory ("${cr_module}/test")
  endforeach (cr_module)
endif (CR_BUILD_TESTS)
