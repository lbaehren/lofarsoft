
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
  
if (PostgreSQL_LIB)
  list (APPEND link_libraries ${PostgreSQL_LIB})
endif (PostgreSQL_LIB)

if (FFTW3_LIBRARIES)
  list (APPEND link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (ROOT_LIBRARIES)
  list (APPEND link_libraries ${ROOT_LIBRARIES})
endif (ROOT_LIBRARIES)

if (libdl)
  list (APPEND link_libraries ${libdl})
endif (libdl)

if (libm)
  list (APPEND link_libraries ${libm})
endif (libm)

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
  RunFlagSelection
  ChannelStatusControl
  RunFlagMergeGrande
  Beamforming
  RunFlagRecEvent
  )

if (PostgreSQL_INCLUDES AND PostgreSQL_LIB) 
  list (APPEND apps db2csv)
  list (APPEND apps RunFlagInDB)
endif (PostgreSQL_INCLUDES AND PostgreSQL_LIB)

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
