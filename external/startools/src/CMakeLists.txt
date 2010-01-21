
## ==============================================================================
##
##  Libraries
##
## ==============================================================================

## ------------------------------------------------------------------------------
## Compiler settings

add_definitions (
  -ggdb3
  -Wall
  -fPIC
  -D_REENTRANT
  )

## ------------------------------------------------------------------------------
## linker settings

set (link_libraries "")
  
if (POSTGRESQL_LIBRARIES)
  list (APPEND link_libraries ${POSTGRESQL_LIBRARIES})
endif (POSTGRESQL_LIBRARIES)

if (FFTW3_LIBRARIES)
  list (APPEND link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (ROOT_LIBRARIES)
  list (APPEND link_libraries ${ROOT_LIBRARIES})
endif (ROOT_LIBRARIES)

if (HAVE_LIBDL)
  list (APPEND link_libraries ${HAVE_LIBDL})
endif (HAVE_LIBDL)

if (HAVE_LIBM)
  list (APPEND link_libraries ${HAVE_LIBM})
endif (HAVE_LIBM)

## ------------------------------------------------------------------------------
## Library 1: Star-Utilities

add_library (starutil
  util.cpp
  trigger.cpp
  calibration.cpp
  reconstruction.cpp
  )

target_link_libraries (starutil ${link_libraries})

## ------------------------------------------------------------------------------
## Library 2: Reconstruction Radio

add_library (recradio
  RecRadioBasic.cpp
  RecRadioCalib.cpp
  RecRadioTrigger.cpp
  RecRadioUtil.cpp
  )

target_link_libraries (recradio ${link_libraries})

## ------------------------------------------------------------------------------
## Installation

## Header files

file (GLOB header_files *.hh)

install (FILES ${header_files}
  DESTINATION include/startools
  )

## Libraries

install (TARGETS starutil recradio
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Applications
##
## ==============================================================================

if (StarTools_BUILD_APPLICATIONS)
  
  ## List of applications to be build
  
  set (apps
    DynamicSpectra
    EventViewer
    SpectraViewer
    RunFlagOut
    RunFlagSelection
    ChannelStatusControl
    RunFlagMergeGrande
    Beamforming
    RunFlagRecEvent
    )
  
  if (NOT APPLE)
    if (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES) 
      list (APPEND apps db2csv)
      list (APPEND apps RunFlagInDB)
    endif (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)
  endif (NOT APPLE)
  
  ## Build instructions for the applications
  
  foreach (app ${apps})
    ## compiler instructions
    add_executable (${app} ${app}.cpp)
    ## linker instructions
    target_link_libraries (${app}
      starutil
      recradio
      )
    ## platform-specific instructions
    if (APPLE)
      set_target_properties (${app}
	PROPERTIES
	LINK_FLAGS "-bind_at_load"
	)
    endif (APPLE)
  endforeach (app)

  ## Installation
  
  install (TARGETS ${apps}
    RUNTIME DESTINATION bin
    )
  
endif (StarTools_BUILD_APPLICATIONS)
