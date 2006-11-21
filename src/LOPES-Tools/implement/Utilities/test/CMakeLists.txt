PROJECT (LOPES-Tools)

include_directories (
  .
  ..
  ../..
  ${CASA_INCLUDE_DIR}
  )

add_definitions (-DHAVE_CASA)

ADD_EXECUTABLE (tProgressBar tProgressBar.cc)
TARGET_LINK_LIBRARIES (tProgressBar lopestools)

ADD_EXECUTABLE (tTimestamp tTimestamp.cc)
TARGET_LINK_LIBRARIES (tTimestamp lopestools)

ADD_EXECUTABLE (tTestArray tTestArray.cc)
TARGET_LINK_LIBRARIES (tTestArray lopestools ${CASA_LIBRARY})
