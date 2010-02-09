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
  ttbbctlIn
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

##_______________________________________________________________________________
##                                                            Testing preparation

if (dataset_tbb)
  
  find_program (BASH_EXCUTABLE bash ${bin_locations})
  get_filename_component (dataset_tbb_local ${dataset_tbb} NAME)
  
  foreach (testScript
      tLOFAR_TBB
      )
    configure_file (${CR_SOURCE_DIR}/implement/IO/test/${testScript}.sh
      ${CR_BINARY_DIR}/implement/IO/test/${testScript}.sh
      )
  endforeach (testScript)
  
endif (dataset_tbb)

##____________________________________________________________________
## Test working with LOPES example dataset

foreach (src tUpSampledDR)
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr)
  ## testing
  if (dataset_lopes_example)
    add_test (${src} ${src} ${dataset_lopes_example})
  else (dataset_lopes_example)
    add_test (${src} ${src})
  endif (dataset_lopes_example)
endforeach (src)

##____________________________________________________________________
##                                                          tLOFAR_TBB

if (HAVE_CASACORE AND DAL_LIBRARIES AND HDF5_LIBRARIES)
  
  ## compiler instructions
  add_executable (tLOFAR_TBB tLOFAR_TBB.cc)
  ## linker instructions
  target_link_libraries (tLOFAR_TBB
    cr
    ${HAVE_LIBZ}
    ${DAL_LIBRARIES}
    ${CASA_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  if (dataset_tbb)
    add_test (tLOFAR_TBB ${BASH_EXCUTABLE} tLOFAR_TBB.sh)
  else (dataset_tbb)
    add_test (tLOFAR_TBB tLOFAR_TBB)
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
