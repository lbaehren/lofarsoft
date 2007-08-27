
## ------------------------------------------------------------------------------
## Test programs for components/ComponentModels

SET (tests
  dPointShape
  dSkyCompBase
  dSpectralModel
  dTwoSidedShape
  tComponentList
  tComponentType
  tConstantSpectrum
  tDiskShape
  tGaussianComponent
  tGaussianShape
  tPointComponent
  tPointShape
  tSkyCompRep
  tSpectralIndex
)

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src}
    casa_components
	casa_coordinates
	casa_fits
	casa_measures
	casa_tables
	casa_scimath
	casa_scimath_f
	casa_casa
	${G2C_LIBRARIES}
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
