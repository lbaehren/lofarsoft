##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

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

if (ROOT_LIBRARIES AND PYTHON_LIBRARIES)
  ## build instructions
  add_executable (tHistograms tHistograms.cc)
  ## link instructions
  if (NOT APPLE)
    target_link_libraries (tHistograms ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} util)
  else (NOT APPLE)
    target_link_libraries (tHistograms ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES})
  endif (NOT APPLE)
  ## testing
  add_test (tHistograms tHistograms)
  ## installation instructions
  list (APPEND tests tHistograms)
endif (ROOT_LIBRARIES AND PYTHON_LIBRARIES)

## [2] Test analysis of a LOPES-event; the programs requires both PGPlot and X11
##     for plotting the results, so it only makes sense building the executable
##     if both are available on the system

if (HAVE_PLPLOT OR HAVE_PGPLOT)
  ## build instructions
  add_executable (tanalyseLOPESevent tanalyseLOPESevent.cc)
  ## [1] When using PLplot that is the only additional library we need to link against
  if (HAVE_PLPLOT)
    ## link instructions
    target_link_libraries (tanalyseLOPESevent cr ${PLPLOT_LIBRARIES})
  endif (HAVE_PLPLOT)
  # [2] When using PGPlot, we also require the X11 libraries
  if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
    ## link instructions
    target_link_libraries (tanalyseLOPESevent cr ${X11_LIBRARIES} ${PGPLOT_LIBRARIES})
  endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)
  ## testing
  add_test (tanalyseLOPESevent tanalyseLOPESevent)
  ## installation instructions
  list (APPEND tests tanalyseLOPESevent)
endif (HAVE_PLPLOT OR HAVE_PGPLOT)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
