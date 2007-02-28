
PROJECT (CASA_coordinates_tCoordinates)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

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
  TARGET_LINK_LIBRARIES (${src} coordinates)
ENDFOREACH (src)
