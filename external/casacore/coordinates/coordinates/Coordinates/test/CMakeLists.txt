
## ------------------------------------------------------------------------------
## Test programs for coordinates/Coordinates

SET (tests
  dCoordinates
  dRemoveAxes
  dWorldMap
  tCoordinate
  tCoordinateSystem
  tCoordinateUtil
  tDirectionCoordinate
  tFrequencyAligner
  tGaussianConvert
  tLinearCoordinate
  tLinearXform
  tObsInfo
  tProjection
  tSpectralCoordinate
  tStokesCoordinate
  tTabularCoordinate
)

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} coordinates fits measures scimath scimath_f casa)
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
