
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_measures_sources *.cc)

add_library (pyrap_measures ${pyrap_measures_sources})

target_link_libraries (pyrap_measures pyrap)

if (APPLE)
  set_target_properties (pyrap_measures
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap_measures
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

