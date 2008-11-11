
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_sources *.cc)

add_library (pyrap ${pyrap_sources})

target_link_libraries (pyrap
  ${CASACORE_LIBRARIES}
  ${BOOST_LIBRARIES}
  )

if (APPLE)
  set_target_properties (pyrap
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

##_______________________________________________________________________________
## Testing

add_subdirectory (test)
