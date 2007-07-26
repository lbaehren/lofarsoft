
## ------------------------------------------------------------------------------
## Test programs for scimath/Fitting

SET (tests
  dConstraints
  dLSQFit
  tFitGaussian
  tLSQFit
  tLSQaips
  tLinearFitSVD
  tNonLinearFitLM
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} scimath casa)
ENDFOREACH (src)
