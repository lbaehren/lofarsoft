
enable_language(Fortran)

#include_directories (${PULSAR_SOURCE_DIR}/apps/presto/src/slalib)

file (GLOB sla_sources *.f)

add_library (sla ${sla_sources})

set_target_properties (sla
  PROPERTIES
  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore -I."
  LINK_FLAGS "-fno-second-underscore"
  )
