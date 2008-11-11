
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_functionals_sources *.cc)

add_library (pyrap_functionals ${pyrap_functionals_sources})

target_link_libraries (pyrap_functionals pyrap)

if (APPLE)
  set_target_properties (pyrap_functionals
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap_functionals
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

