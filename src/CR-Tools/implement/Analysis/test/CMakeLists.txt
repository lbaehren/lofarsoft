##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Test programs that do not require special attention
##
## ==============================================================================

##____________________________________________________________________
## Set up the list with test program sources

set (tests
  tClippingFraction
  tCRdelayPlugin
  tFirstStagePipeline
  tionoCalibration
  tSecondStagePipeline
  tSubbandID
  )

if (CR_WITH_LEGACY) 
  list (APPEND tests tfitCR2gauss)
endif (CR_WITH_LEGACY)

##____________________________________________________________________
## Tests which require input data

if (data_modelcoeff1 AND data_modelcoeff2)
  list (APPEND tests tinverseFFT)
else (data_modelcoeff1 AND data_modelcoeff2)
  message (STATUS "Excluding tinverseFFT from list of tests; missing input data.")
endif (data_modelcoeff1 AND data_modelcoeff2)

if (data_ppf_coefficients AND data_ppf_inversion)
  list (APPEND test tppfimplement)
else (data_ppf_coefficients AND data_ppf_inversion)
  message (STATUS "Excluding tppfimplement from list of tests; missing input data.")
endif (data_ppf_coefficients AND data_ppf_inversion)

##____________________________________________________________________
## Build the test programs

foreach (src ${tests})
  
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src}
    cr
    ${CASA_LIBRARIES})
  ## testing
  add_test (${src} ${src})

endforeach (src)

## ==============================================================================
##
##  Test programs that require special attention
##
## ==============================================================================

##____________________________________________________________________
## Test programs which require the DAL

if (dataset_beamformed)
  add_test (tppfinversion tppfinversion ${dataset_beamformed})
endif (dataset_beamformed)

if (data_ppf_coefficients AND data_ppf_inversion)
  
  ## compiler instructions
  add_executable (tppfimplement tppfimplement.cc)
  ## linker instructions
  target_link_libraries (tppfimplement cr)
  ## testing
  add_test (tppfimplement tppfimplement)

endif (data_ppf_coefficients AND data_ppf_inversion)

##____________________________________________________________________
## Test analysis of a LOPES-event; the programs requires both PGPlot
## and X11 for plotting the results, so it only makes sense building
## the executable if both are available on the system

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

if (CR_WITH_STARTOOLS)
  ## compiler instructions
  add_executable (tanalyseLOPESevent2
    tanalyseLOPESevent2.cc
    ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C
    )
  ## linker instructions
  if (HAVE_PLPLOT OR HAVE_PGPLOT)
    target_link_libraries (tanalyseLOPESevent2
      cr
      ${STARTOOLS_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${PLPLOT_LIBRARIES}
      ${CASA_LIBRARIES}
      ${WCSLIB_LIBRARIES}
      )
  else (HAVE_PLPLOT OR HAVE_PGPLOT)
    target_link_libraries (tanalyseLOPESevent2
      cr
      ${STARTOOLS_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${CASA_LIBRARIES}
      ${WCSLIB_LIBRARIES}
      )
  endif (HAVE_PLPLOT OR HAVE_PGPLOT)
  if (PGPLOT_LIBRARIES AND X11_LIBRARIES)
    ## link instructions
    target_link_libraries (tanalyseLOPESevent2
      cr
      ${STARTOOLS_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      ${X11_LIBRARIES}
      ${PGPLOT_LIBRARIES}
      )
  endif (PGPLOT_LIBRARIES AND X11_LIBRARIES)
  ## testing 
  add_test (tanalyseLOPESevent2 tanalyseLOPESevent2)
  ## installation
  list (APPEND tests tanalyseLOPESevent2)
endif (CR_WITH_STARTOOLS)

##____________________________________________________________________
## Compute and plot station beam

if (CR_WITH_PLOTTING)

  ## compiler instructions
  add_executable (tStationBeam tStationBeam.cc)
  ## linker instructions
  target_link_libraries (tStationBeam cr)
  if (APPLE)
    set_target_properties (tStationBeam
      PROPERTIES
      LINK_FLAGS "-bind_at_load"
      )
  endif (APPLE)
  ## installation 
  list (APPEND tests tStationBeam)
  ## testing
  if (CR_ENABLE_TESTING) 
    add_test (tStationBeam tStationBeam)
  endif (CR_ENABLE_TESTING) 
  
endif (CR_WITH_PLOTTING)

##____________________________________________________________________
## tcheckNoiseInfluence

if (HAVE_STARTOOLS AND CR_WITH_PLOTTING AND HAVE_ROOT)

  ## compiler instructions
  add_executable (tcheckNoiseInfluence tcheckNoiseInfluence.cc)
  ## linker instructions
  if (HAVE_LIBM AND HAVE_LIBUTIL)
    target_link_libraries (tcheckNoiseInfluence
      cr
      ${POSTGRESQL_LIBRARIES}
      ${STARTOOLS_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${HAVE_LIBUTIL}
      ${HAVE_LIBM}
      )
  else (HAVE_LIBM AND HAVE_LIBUTIL)
    target_link_libraries (tcheckNoiseInfluence
      cr
      ${POSTGRESQL_LIBRARIES}
      ${STARTOOLS_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      )
  endif (HAVE_LIBM AND HAVE_LIBUTIL)
  ## extra properties to take care of
  if (APPLE)
    set_target_properties (tcheckNoiseInfluence
      PROPERTIES
      LINK_FLAGS "-bind_at_load"
      )
  endif (APPLE)
  ## testing
  if (CR_ENABLE_TESTING) 
    add_test (tcheckNoiseInfluence tcheckNoiseInfluence)
  endif (CR_ENABLE_TESTING) 
  ## installation
  list (APPEND tests tcheckNoiseInfluence)

endif (HAVE_STARTOOLS AND CR_WITH_PLOTTING AND HAVE_ROOT)

##____________________________________________________________________
##                                                    tAntennasDisplay
##                                                    tPulseProperties

if (HAVE_STARTOOLS AND HAVE_DAL)

  foreach (src
      tAntennasDisplay
      tPulseProperties
      tppfinversion
      tgenCRmap
      tCompletePipeline
      )
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src}
      cr
      ${POSTGRESQL_LIBRARIES}
      ${STARTOOLS_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      ${HAVE_LIBUTIL}
      ${HAVE_LIBM}
      )
    ## testing
    add_test (${src} ${src})
    ## installation
    list (APPEND tests ${src})
  endforeach (src)
  
endif (HAVE_STARTOOLS AND HAVE_DAL)

## ==============================================================================
##
##  Testing
##
## ==============================================================================

if (data_ppf_coefficients AND data_ppf_inversion)
  add_test (tppfimplement tppfimplement)  
endif (data_ppf_coefficients AND data_ppf_inversion)


## ==============================================================================
##
##  Installation
##
## ==============================================================================

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
