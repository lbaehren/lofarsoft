##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tAntenna
  tCoordinateConversion
  tFeed
  tGeometricalDelay
  tGeometricalPhase
  tGeometricalWeight
  tMDirection
  tSkymapCoordinates
  tSkymapGrid
  tSkymapper
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
endforeach (src)

