##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tAntenna
  tBeamformer
  tFeed
  tGeometricalDelay
  tGeometricalPhase
  tGeometricalWeight
  tMDirection
  tSkymapCoordinates
  tSkymapGrid
  )

if (positions_lopes08)
  list (APPEND tests tSkymapper)
else (positions_lopes08)
  message (STATUS "Excluding tSkymapper from list of tests; missing input data.")
endif (positions_lopes08)

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
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
