
project (CR)

include (${CR_SOURCE_DIR}/../../devel_common/cmake/FindBlitz.cmake)

include_directories (
  .
  ..
  ${BLITZ_INCLUDE_DIR}
  )

subdirs (
  apps
  implement
  test
  )
