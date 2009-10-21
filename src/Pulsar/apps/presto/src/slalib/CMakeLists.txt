
enable_language(Fortran)

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/src/slalib)

file (GLOB sla_sources *.f)
list (APPEND sla_sources gresid.F)
list (APPEND sla_sources random.F)

add_library (sla ${sla_sources})

set_target_properties (sla
  PROPERTIES
  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
  LINK_FLAGS "-fno-second-underscore"
  )

install (
  TARGETS sla
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib
  )
