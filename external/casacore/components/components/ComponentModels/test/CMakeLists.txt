
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

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  if (GFORTRAN_LIBRARIES)
    target_link_libraries (${src}
      casa_components
      casa_coordinates
      casa_fits
      casa_measures
      casa_tables
      casa_scimath
      casa_scimath_f
      casa_casa
      ${GFORTRAN_LIBRARIES}
      )
  else (GFORTRAN_LIBRARIES)
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
  endif (GFORTRAN_LIBRARIES)
  ## testing 
  add_test (${src} ${src})
endforeach (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
