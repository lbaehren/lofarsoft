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
  tSimulator
  tPolyPhaseFilter
  )

if (CR_WITH_LEGACY) 
#  list (APPEND tests tfitCR2gauss)
#  list (APPEND tests tCRinvFFT)
endif (CR_WITH_LEGACY)

##____________________________________________________________________
## Tests which require input data

#if (data_modelcoeff1 AND data_modelcoeff2)
#  list (APPEND tests tinverseFFT)
#else (data_modelcoeff1 AND data_modelcoeff2)
#  message (STATUS "Excluding tinverseFFT from list of tests; missing input data.")
#endif (data_modelcoeff1 AND data_modelcoeff2)

#if (data_ppf_coefficients AND data_ppf_inversion)
#  list (APPEND test tppfimplement)
#else (data_ppf_coefficients AND data_ppf_inversion)
#  message (STATUS "Excluding tppfimplement from list of tests; missing input data.")
#endif (data_ppf_coefficients AND data_ppf_inversion)

##____________________________________________________________________
## Build the test programs

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr ${CASA_LIBRARIES})
  ## testing
  add_test (${src} ${src})
endforeach (src)

## ==============================================================================
##
##  Test programs that require special attention
##
## ==============================================================================

##____________________________________________________________________
## Test programs which require HDF5 library

#if (HAVE_HDF5)
  ## compiler instructions
#  add_executable (tgenCRmap tgenCRmap.cc)
  ## linker instructions
#  target_link_libraries (tgenCRmap cr ${HDF5_LIBRARIES})
  ## testing
#  add_test (tgenCRmap tgenCRmap)
#endif (HAVE_HDF5)

##____________________________________________________________________
## Test programs which require the DAL

if (HAVE_DAL AND HAVE_ROOT)
  ## compiler instructions
  add_executable (tTBBData tTBBData.cc)
  ## linker instructions
  target_link_libraries (tTBBData
    cr
    ${ROOT_LIBRARIES}
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  add_test (tTBBData tTBBData)
  ## installation
  list (APPEND tests tTBBData)
  ## compiler instructions
  add_executable (tTBBAnalysis tTBBAnalysis.cc)
  ## linker instructions
  target_link_libraries (tTBBAnalysis
    cr
    ${ROOT_LIBRARIES}
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  add_test (tTBBAnalysis tTBBAnalysis)
  ## installation
  list (APPEND tests tTBBAnalysis)
endif (HAVE_DAL AND HAVE_ROOT)



## ==============================================================================
##
##  Testing
##
## ==============================================================================

#if (data_ppf_coefficients AND data_ppf_inversion)
#  add_test (tppfimplement tppfimplement)  
#endif (data_ppf_coefficients AND data_ppf_inversion)


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
