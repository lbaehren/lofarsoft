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
  if (libutil)
    target_link_libraries (tHistograms ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} ${libutil})
  else (libutil)
    target_link_libraries (tHistograms ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES})
  endif (libutil)
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
  add_executable (tanalyseLOPESevent2 tanalyseLOPESevent2.cc)
  ## [1] When using PLplot that is the only additional library we need to link against
  if (HAVE_PLPLOT)
    ## link instructions
    target_link_libraries (tanalyseLOPESevent
      cr
      ${PLPLOT_LIBRARIES}
      )
    target_link_libraries (tanalyseLOPESevent2
      cr
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${PLPLOT_LIBRARIES}
      )
  endif (HAVE_PLPLOT)
  # [2] When using PGPlot, we also require the X11 libraries
  if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
    ## link instructions
    target_link_libraries (tanalyseLOPESevent cr ${X11_LIBRARIES} ${PGPLOT_LIBRARIES})
    target_link_libraries (tanalyseLOPESevent2 cr ${X11_LIBRARIES} ${PGPLOT_LIBRARIES})
  endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)
  ## testing
  add_test (tanalyseLOPESevent tanalyseLOPESevent)
  add_test (tanalyseLOPESevent2 tanalyseLOPESevent2)
  ## installation instructions
  list (APPEND tests tanalyseLOPESevent)
  list (APPEND tests tanalyseLOPESevent2)
endif (HAVE_PLPLOT OR HAVE_PGPLOT)

## [3] Test programs for programs using functionality of LopesStar

if (HAVE_ROOT AND HAVE_FFTW3)
  ## build executable
  add_executable (tCompletePipeline tCompletePipeline.cc)
  target_link_libraries (tCompletePipeline
    cr
    ${FFTW3_LIBRARIES}
    ${ROOT_LIBRARIES}
    ${PYTHON_LIBRARIES}
    )
  ## testing
  add_test (tCompletePipeline tCompletePipeline)
endif (HAVE_ROOT AND HAVE_FFTW3)


## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
