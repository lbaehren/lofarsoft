##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Test programs that do not require special attention
##
## ==============================================================================

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

## Build the test programs

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr)
  ## testing
  if (CR_ENABLE_TESTING) 
    add_test (${src} ${src})
  endif (CR_ENABLE_TESTING) 
endforeach (src)

## ==============================================================================
##
##  Test programs that require special attention
##
## ==============================================================================

## -------------------------------------------------------------------

if (NOT APPLE)

  ## [1] Test analysis of a LOPES-event; the programs requires both PGPlot and X11
  ##     for plotting the results, so it only makes sense building the executable
  ##     if both are available on the system
  
  if (HAVE_PLPLOT OR HAVE_PGPLOT)
    ## build instructions
    add_executable (tanalyseLOPESevent tanalyseLOPESevent.cc)
    ## [1] When using PLplot that is the only additional library we need to link
    ##     against
    if (HAVE_PLPLOT)
      ## link instructions
      target_link_libraries (tanalyseLOPESevent
	cr
	${PLPLOT_LIBRARIES}
	)
    endif (HAVE_PLPLOT)
    ## [3] When using PGPlot, we also require the X11 libraries
    if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
      ## link instructions
      target_link_libraries (tanalyseLOPESevent
	cr
	${X11_LIBRARIES}
	${PGPLOT_LIBRARIES}
	)
    endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)
    ## testing
    add_test (tanalyseLOPESevent tanalyseLOPESevent)
    ## installation instructions
    list (APPEND tests tanalyseLOPESevent)
  endif (HAVE_PLPLOT OR HAVE_PGPLOT)
  
  if (CR_ENABLE_LOPESSTAR)
    ## compiler instructions
    add_executable (tanalyseLOPESevent2 tanalyseLOPESevent2.cc)
    ## linker instructions
    if (HAVE_PLPLOT OR HAVE_PGPLOT)
      target_link_libraries (tanalyseLOPESevent2
	cr
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PLPLOT_LIBRARIES}
	${CASA_LIBRARIES}
	)
    endif (HAVE_PLPLOT OR HAVE_PGPLOT)
    if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
      ## link instructions
      target_link_libraries (tanalyseLOPESevent2
	cr
	${X11_LIBRARIES}
	${PGPLOT_LIBRARIES}
	)
    endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)
    ## testing 
    add_test (tanalyseLOPESevent2 tanalyseLOPESevent2)
    ## installation
    list (APPEND tests tanalyseLOPESevent2)
  endif (CR_ENABLE_LOPESSTAR)
  
  ## [3] Test programs for programs using functionality of LopesStar
  
  if (CR_ENABLE_LOPESSTAR AND CR_PLOTTING_SUPPORT)
    ## compiler instructions
    add_executable (tCompletePipeline tCompletePipeline.cc)
    ## linker instructions
    if (libm AND libutil)
      target_link_libraries (tCompletePipeline
	cr
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${libutil}
	${libm}
	)
    else (libm AND libutil)
      target_link_libraries (tCompletePipeline
	cr
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	)
    endif (libm AND libutil)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (tCompletePipeline
	PROPERTIES
	LINK_FLAGS "-bind_at_load"
	)
    endif (APPLE)
    ## testing
    if (CR_ENABLE_TESTING) 
      add_test (tCompletePipeline tCompletePipeline)
    endif (CR_ENABLE_TESTING) 
  endif (CR_ENABLE_LOPESSTAR AND CR_PLOTTING_SUPPORT)
  
endif (NOT APPLE)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
