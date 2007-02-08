
FILE (GLOB pgplot_sys_sources *.f *.c)

## build the library

set_source_files_properties (${pgplot_sys_sources}
  PROPERTIES COMPILE_FLAGS "-fPIC -O2 -fPIC")

add_library (pgplot_sys
  ${pgplot_sys_sources}
  )
