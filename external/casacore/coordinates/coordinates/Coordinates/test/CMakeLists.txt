
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
  TARGET_LINK_LIBRARIES (${src}
    casa_coordinates
	casa_fits
	casa_measures
	casa_scimath
	casa_scimath_f
	casa_casa
	)
  ## testing 
  add_test (${src} ${src})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
