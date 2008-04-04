
##-------------------------------------------------------------------------------
## $Id                                                                          $
##-------------------------------------------------------------------------------

## Additional compiler settings

include_directories (
  ${StarTools_SOURCE_DIR}/src
  )

## Additional linker settings

set (LopesStar_link_libraries
  cr
  )

if (FFTW3_LIBRARIES)
  list (APPEND LopesStar_link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (ROOT_LIBRARIES)
  list (APPEND LopesStar_link_libraries ${ROOT_LIBRARIES})
endif (ROOT_LIBRARIES)

if (PYTHON_LIBRARIES)
  list (APPEND LopesStar_link_libraries ${PYTHON_LIBRARIES})
endif (PYTHON_LIBRARIES)

## -----------------------------------------------------------------------------
## Applications

set (LopesStar_apps
  DynamicSpectra
  EventViewer
  SpectraViewer
  RunFlagOut
  RunFlagSelection
  ChannelStatusControl
  )

foreach (src ${LopesStar_apps})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} ${LopesStar_link_libraries})
endforeach (src)

## programs which need special treatment

if (PostgreSQL_LIBRARIES AND PostgreSQL_INCLUDES)
  ## compiler instructions
  add_executable (RunFlagInDB RunFlagInDB.cc)
  ##
  set_target_properties (RunFlagInDB
    PROPERTIES
    COMPILE_FLAGS "-DHAVE_PostgreSQL -I${PostgreSQL_INCLUDES}"
    )
  ## linker instructions
  target_link_libraries (RunFlagInDB cr ${PostgreSQL_LIBRARIES})
  ## installation
  list (APPEND LopesStar_apps RunFlagInDB)
endif (PostgreSQL_LIBRARIES AND PostgreSQL_INCLUDES)

## ------------------------------------------------------------------------------
## Installation

install (
  TARGETS ${LopesStar_apps}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
