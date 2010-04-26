##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tAntennaArray
  tAntennaArrays
  tCoordinateType
  tCoordinateGrid
  tGridCoordinate
  tMConversions
  tPolarCoordinate
  tPositionVector
  tSkymapCoordinate
  tSkymapQuantity
  tSpatialCoordinate
  tTimeFreq
  tTimeFreqCoordinate
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr ${cr_link_libraries})
  ## testing
  add_test (${src} ${src})
endforeach (src)

## ------------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

