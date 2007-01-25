
project (CR)

include (../../common/FindBlitz.cmake)

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
