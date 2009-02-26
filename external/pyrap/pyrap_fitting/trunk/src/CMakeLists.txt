
##_______________________________________________________________________________
## Build the shared library

file (GLOB pyrap_fitting_sources *.cc)

add_library (pyrap_fitting ${pyrap_fitting_sources})

target_link_libraries (pyrap_fitting pyrap)

if (APPLE)
  set_target_properties (pyrap_fitting
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (pyrap_fitting
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
  TARGETS pyrap_fitting
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

