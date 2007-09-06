
##-------------------------------------------------------------------------------
## $Id                                                                          $
##-------------------------------------------------------------------------------

include_directories (
  ${StarTools_SOURCE_DIR}/src
  ${FFTW3_INCLUDES}
  ${ROOT_INCLUDES}
  )

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
  target_link_libraries (${src}
    cr
    ${FFTW3_LIBRARIES}
    ${ROOT_LIBRARIES}
    )
endforeach (src)

## programs which need special treatment

if (PostgreSQL_LIBRARIES)
  ## compiler instructions
  add_executable (RunFlagInDB RunFlagInDB.cc)
  ## linker instructions
  target_link_libraries (RunFlagInDB cr ${PostgreSQL_LIBRARIES})
  ## installation
  list (APPEND LopesStar_apps RunFlagInDB)
endif (PostgreSQL_LIBRARIES)

## ------------------------------------------------------------------------------
## Installation

install (
  TARGETS ${LopesStar_apps}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
