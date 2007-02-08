
FILE (GLOB pgplot_src_sources *.f *.c)

## build the library

set_source_files_properties (${pgplot_src_sources}
  PROPERTIES COMPILE_FLAGS "-fPIC -O2 -fPIC")

add_library (pgplot_src
  ${pgplot_src_sources}
  )
