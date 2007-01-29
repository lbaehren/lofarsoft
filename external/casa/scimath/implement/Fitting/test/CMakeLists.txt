
PROJECT (CASA_scimath_tFitting)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  dConstraints
  dLSQFit
  tFitGaussian
  tLinearFitSVD
  tLSQaips
  tLSQFit
  tNonLinearFitLM
  )

## ------------------------------------------------------------------------------
## Build the test programs
##
## XLIBLIST := LAPACK F77

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} scimath casa ${LAPACK_LIBRARY})
ENDFOREACH (src)

