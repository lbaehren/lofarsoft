##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tBasicFilter
  tHanningFilter
  tHermite1D
  tLinearRegression
  tMatrixOperations
  tMedianFilter
  tRaisedCosineFilter
  tRangeSplitter
  tShapelets1D
  tShapelets2D
  tShapeletsModel1D
  tShapeletsModel2D
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
