##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Tests without extra requirements
##
## ==============================================================================

##____________________________________________________________________
## Set up the list with test program sources

set (tests
  tData
  tDataFlag
  tDataType
  tITS_Beam
  tITSMetadata
  tLopesEvent
  tLopesEventIn
  tNDABeam
  tObservationData
  tTIM40Header
  ttbbctlIn
  trawSubbandIn
  tUpSampledDR
  )

##____________________________________________________________________
## Build the test programs

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

## ==============================================================================
##
##  Tests without extra requirements
##
## ==============================================================================

##____________________________________________________________________
## tLOFAR_TBB

if (HAVE_CASACORE AND DAL_LIBRARIES AND HDF5_LIBRARIES)
  
  ## compiler instructions
  add_executable (tLOFAR_TBB tLOFAR_TBB.cc)
  ## linker instructions
  target_link_libraries (tLOFAR_TBB
    cr
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  if (dataset_tbb)
    add_test (tLOFAR_TBB tLOFAR_TBB ${dataset_tbb})
  else (dataset_tbb)
    message (STATUS "Unable to run test tLOFAR_TBB; missing input data file.")
  endif (dataset_tbb)
  ## installation
  list (APPEND tests tLOFAR_TBB)
  
endif (HAVE_CASACORE AND DAL_LIBRARIES AND HDF5_LIBRARIES)

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
