##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tData
  tDataFlag
  tITS_Beam
  tITSMetadata
  tLOFAR_Attributes
  tLopesEvent
  tLopesEventIn
  tNDABeam
  tTIM40Header
  ttbbctlIn
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
## Build the test programs working with the Data Access Library (DAL)

if (DAL_LIBRARIES AND HDF5_LIBRARIES)
  
  find_file (tbb_dataset tbb_test_file_v2.h5
    PATHS
    ${CR_SOURCE_DIR}/data
    ${CR_SOURCE_DIR}/../data
    ${CR_SOURCE_DIR}/../../data
    )
  
  foreach (src tHDF5Common tLOFAR_StationGroup tLOFAR_TBB) 
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src} cr ${DAL_LIBRARIES} ${HDF5_LIBRARIES})
    ## testing
    if (tbb_dataset)
      add_test (${src} ${src} ${tbb_dataset})
    else (tbb_dataset)
      message (STATUS "Unable to run test ${src}; missing input data file.")
    endif (tbb_dataset)
  endforeach (src)

endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
  
## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
