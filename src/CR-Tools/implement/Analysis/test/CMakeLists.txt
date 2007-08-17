##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tClippingFraction
  tCRdelayPlugin
  tCRinvFFT
  tDynamicSpectrum
  tFirstStagePipeline
  tfitCR2gauss
  tgenCRmap
  tinverseFFT
  tionoCalibration
  tppfimplement
  tppfinversion
  tSecondStagePipeline
  tSubbandID
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
## Additional test programs which require extra care

## [1] Test working with ROOT histograms; this requires ROOT to be installed on
##     the system

if (ROOT_LIBRARIES)
  ## build instructions
  add_executable (tHistograms tHistograms.cc)
  ## link instructions
  target_link_libraries (tHistograms ${ROOT_LIBRARIES})
  ## testing
  add_test (tHistograms tHistograms)
  ## installation instructions
  list (APPEND tests tHistograms)
endif (ROOT_LIBRARIES)

## [2] Test analysis of a LOPES-event; the programs requires both PGPlot and X11
##     for plotting the results, so it only makes sense building the executable
##     if both are available on the system

if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
  ## build instructions
  add_executable (tanalyseLOPESevent tanalyseLOPESevent.cc)
  ## link instructions
  target_link_libraries (tanalyseLOPESevent cr ${X11_LIBRARIES} ${PGPLOT_LIBRARIES})
  ## testing
  add_test (tanalyseLOPESevent tanalyseLOPESevent)
  ## installation instructions
  list (APPEND tests tanalyseLOPESevent)
endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
