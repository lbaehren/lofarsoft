##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tObservationData
  tObservationFrame
  )

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

## -----------------------------------------------------------------------------
## Test programs which need extra attention

if (CR_WITH_PLOTTING)

  ## compiler instructions
  add_executable (tStationBeam tStationBeam.cc)
  ## linker instructions
  if (PLPLOT_LIBRARIES)
    target_link_libraries (tStationBeam
      cr
      ${PLPLOT_LIBRARIES}
      ${X11_LIBRARIES}
      )
  endif (PLPLOT_LIBRARIES)
  if (APPLE)
    set_target_properties (tStationBeam
      PROPERTIES
      LINK_FLAGS "-bind_at_load"
      )
  endif (APPLE)
  ## installation 
  list (APPEND tests tStationBeam)
  ## testing
  if (CR_ENABLE_TESTING) 
    add_test (tStationBeam tStationBeam)
  endif (CR_ENABLE_TESTING) 
  
endif (CR_WITH_PLOTTING)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
