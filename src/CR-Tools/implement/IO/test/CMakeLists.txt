##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

##_______________________________________________________________________________
##                                                           List of source files

file (GLOB tests_IO *.cc)

if (NOT HAVE_CASACORE OR NOT DAL_LIBRARIES OR NOT HDF5_LIBRARIES)
  list (REMOVE_ITEM tests_IO ${CURRENT_SOURCE_DIR}/LOFAR_TBB.cc)
endif (NOT HAVE_CASACORE OR NOT DAL_LIBRARIES OR NOT HDF5_LIBRARIES)

##_______________________________________________________________________________
##                                                     Build/Install instructions

foreach (src ${tests_IO})
  ## get the name of the file without the extension
  get_filename_component (src ${src} NAME_WE)
  ## compiler instructions
  add_executable(${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src}
    cr
    ${cr_link_libraries}
    )
  ## installation instructions
  install (TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
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

##_______________________________________________________________________________
##                                                           Testing instructions

foreach (test
    tApplyInstrumentEffects
    tDataBlockSettings
    tDataIterator
    tDataReader
    tFFTServer
    tFalckeGorhamGalacticNoise
    tITSMetadata
    tImportAntenna
    tImportSimulation
    tLogfileName
    tObservationData
    tRawSubbandIn
    tSimulation2fftMatrix
    tTSmatrix2Event
    tgenTVSpec
    tsimulation2event
    ttbbctlIn
    ttoLOPESgrid
    )
  add_test (${test} ${test})
endforeach (test)

## Tests which need additional input
    
if (dataset_lopes_example)
  add_test (tUpSampledDR tUpSampledDR ${dataset_lopes_example})
else (dataset_lopes_example)
  add_test (tUpSampledDR tUpSampledDR)
endif (dataset_lopes_example)

if (dataset_tbb)
  add_test (tLOFAR_TBB ${BASH_EXCUTABLE} tLOFAR_TBB.sh)
else (dataset_tbb)
  add_test (tLOFAR_TBB tLOFAR_TBB)
endif (dataset_tbb)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
