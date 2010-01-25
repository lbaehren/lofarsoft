##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tBasicFilter
  tHanningFilter
  tLinearRegression
  tMatrixOperations
  tMedianFilter
  tRaisedCosineFilter
  tRangeSplitter
  tStatisticsFilter
  tThreeVector
  tVectorConversion
  tVectorNorms
  )

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
