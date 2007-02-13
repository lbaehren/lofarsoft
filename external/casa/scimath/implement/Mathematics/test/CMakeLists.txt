
PROJECT (CASA_scimath_tMathematics)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  dAutoDiff
  tAutoDiff
  tConvolver
  tFFTServer
  tHistAcc
  tMathFunc
  tMatrixMathLA
  tMedianSlider
  tPrimes
  tStatAcc
  tVanVleck
  )

## ------------------------------------------------------------------------------
## Build the test programs
##
## XLIBLIST := LAPACK F77

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} scimath casa ${LAPACK_LIBRARY})
ENDFOREACH (src)

