
project (CR)

include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindBlitz.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCASA.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindCFITSIO.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindFFTW3.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindLAPACK.cmake)
include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindROOT.cmake)

include_directories (
  .
  ..
  ./implement
  ./test
  ${BLITZ_INCLUDES}
  ${ROOT_INCLUDES}
  )

subdirs (
  apps
  implement
  test
  )

SET (CMAKE_VERBOSE_MAKEFILE 1)
