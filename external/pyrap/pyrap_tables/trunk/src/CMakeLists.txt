
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_tables_sources *.cc)

add_library (pyrap_tables ${pyrap_tables_sources})

target_link_libraries (pyrap_tables pyrap)

if (APPLE)
  set_target_properties (pyrap_tables
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap_tables
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

##_______________________________________________________________________________
## Installation

install (
  TARGETS pyrap_tables
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
