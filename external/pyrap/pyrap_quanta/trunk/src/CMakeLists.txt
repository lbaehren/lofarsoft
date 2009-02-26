
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_quanta_sources *.cc)

add_library (pyrap_quanta ${pyrap_quanta_sources})

target_link_libraries (pyrap_quanta pyrap)

if (APPLE)
  set_target_properties (pyrap_quanta
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap_quanta
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
  TARGETS pyrap_quanta
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
