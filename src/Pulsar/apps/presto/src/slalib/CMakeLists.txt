
project (libsla C Fortran)

file (GLOB sla_sources *.f)

add_library (sla ${sla_sources})

set_target_properties (sla
  PROPERTIES
  COMPILE_FLAGS "-fPIC -O2 -fPIC"
  )
