
enable_language(Fortran)

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/src/fortran)

file (GLOB fort_sources least_squares.f)

add_library (fort ${fort_sources})

set_target_properties (fort
  PROPERTIES
  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
  LINK_FLAGS "-fno-second-underscore"
  )

install (
  TARGETS fort
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
