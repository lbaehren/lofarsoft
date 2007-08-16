##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tClippingFraction
  tDynamicSpectrum
#  tFirstStagePipeline
  tinverseFFT
#  tSecondStagePipeline
  )

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## buidl executable
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

## -----------------------------------------------------------------------------
## Additional test programs which require extra care

if (ROOT_LIBRARIES)
  ## build instructions
  add_executable (tHistograms tHistograms.cc)
  ## link instructions
  target_link_libraries (tHistograms ${ROOT_LIBRARIES})
  ## testing
  add_test (tHistograms tHistograms)
  ## installation instructions
  install (
    TARGETS tHistograms
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif (ROOT_LIBRARIES)

