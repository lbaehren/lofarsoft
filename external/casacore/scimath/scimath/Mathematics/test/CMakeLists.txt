
## ------------------------------------------------------------------------------
## Test programs for scimath/Mathematics

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
#  tVanVleck
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} scimath scimath_f casa ${LAPACK_LIBRARIES} ${G2C_LIBRARIES})
ENDFOREACH (src)
