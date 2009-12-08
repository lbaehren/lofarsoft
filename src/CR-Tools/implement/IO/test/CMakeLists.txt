##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tApplyInstrumentEffects
  tDataBlockSettings
  tDataIterator
  tDataReader
  tFFTServer
  tgenTVSpec
  tImportAntenna
  tImportSimulation
  tFalckeGorhamGalacticNoise
  tLogfileName
  tsimulation2event
  tSimulation2fftMatrix
  ttoLOPESgrid
  tTSmatrix2Event
  tRawSubbandIn
  )

if (HAVE_HDF5)
  list (APPEND tests tH5ImportConfig)
endif (HAVE_HDF5)

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

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


## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
