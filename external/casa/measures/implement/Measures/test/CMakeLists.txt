
PROJECT (CASA_measures_tMeasures)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

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
  TARGET_LINK_LIBRARIES (${src} measures)
ENDFOREACH (src)

