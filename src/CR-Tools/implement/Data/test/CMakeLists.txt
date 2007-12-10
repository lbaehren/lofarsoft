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
## Build the test programs working with input data sets

add_executable (tLOFAR_TBB tLOFAR_TBB.cc)
target_link_libraries (tLOFAR_TBB cr)

find_file (tbb_dataset tbb_test_file_v2.h5
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  )

if (tbb_dataset)
  add_test (tLOFAR_TBB tLOFAR_TBB ${tbb_dataset})
else (tbb_dataset)
  message (STATUS "Unable to run tLOFAR_TBB; no dataset found!")
endif (tbb_dataset)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
