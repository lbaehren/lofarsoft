
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
  if (GFORTRAN_LIBRARIES) 
    TARGET_LINK_LIBRARIES (${src}
      casa_scimath
      ${LAPACK_LIBRARIES}
      ${GFORTRAN_LIBRARIES}
      )
  else (GFORTRAN_LIBRARIES)
    TARGET_LINK_LIBRARIES (${src}
      casa_scimath
      ${LAPACK_LIBRARIES}
      ${G2C_LIBRARIES}
      )
  endif (GFORTRAN_LIBRARIES)
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
