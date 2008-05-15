
## ==============================================================================
##
##  Libraries
##
## ==============================================================================

## Compiler settings

add_definitions (
  -ggdb3
  -Wall
  -fPIC
  -D_REENTRANT
  )

## Library 1: Star-Utilities

add_library (starutil
  util.cpp
  trigger.cpp
  calibration.cpp
  reconstruction.cpp
  )

if (PostgreSQL_LIB)
  target_link_libraries (starutil
    ${ROOT_LIBRARIES}
    ${FFTW3_LIB}
    ${PostgreSQL_LIB}
    )
else (PostgreSQL_LIB)
  target_link_libraries (starutil
    ${ROOT_LIBRARIES}
    ${FFTW3_LIB}
    )
endif (PostgreSQL_LIB)

## Library 2: Reconstruction Radio

add_library (recradio
  RecRadioBasic.cpp
  RecRadioCalib.cpp
  RecRadioTrigger.cpp
  RecRadioUtil.cpp
  )

## ==============================================================================
##
##  Applications
##
## ==============================================================================

## List of applications to be build

set (apps
  DynamicSpectra
  EventViewer
  SpectraViewer
  RunFlagOut
  RunFlagInDB
  RunFlagSelection
  ChannelStatusControl
  RunFlagMergeGrande
  Beamforming
  RunFlagRecEvent
  )

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

## ==============================================================================
##
##  Installation
##
## ==============================================================================

## Installation of header files

install (FILES util.hh trigger.hh calibration.hh reconstruction.hh
  DESTINATION include/lopes-star
  )

## Installation of the library and the executables

install (TARGETS starutil recradio ${apps}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
