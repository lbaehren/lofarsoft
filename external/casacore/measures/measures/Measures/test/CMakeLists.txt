
## ------------------------------------------------------------------------------
## Test programs for measures/Measures

SET (tests
  dMeasure
  tEarthField
  tEarthMagneticMachine
  tIAU2000
  tMBaseline
  tMEarthMagnetic
  tMeasComet
  tMeasJPL
  tMeasMath
  tMeasure
  tMeasureHolder
  tMuvw
  tParAngleMachine
  tRecordTransformable
  tStokes
  tUVWMachine
  tVelocityMachine
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} sofa casa_measures casa_tables casa_scimath casa_casa)
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
