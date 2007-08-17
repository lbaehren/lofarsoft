
## ------------------------------------------------------------------------------
## Test programs for scimath/Functionals

SET (tests
  dFunction
  dGaussianND
  tChebyshev
  tCombiFunction
  tCompoundFunction
  tFuncExpression
  tFunctionHolder
  tFunctionOrder
  tFunctionWrapper
  tGaussian1D
  tGaussian2D
  tGaussian3D
  tGaussianND
  tHyperPlane
  tInterpolate1D
  tPolynomial
  tSPolynomial
  tSampledFunctional
  tSimButterworthBandpass
  tSinusoid1D
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} scimath casa)
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
