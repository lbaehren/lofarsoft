
add_library (ppgplot ppgplot.c)

## additional target properties
if (APPLE)
  set_target_properties (ppgplot
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON -fpermissive"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (ppgplot
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

target_link_libraries (ppgplot
  ${PGPLOT_LIBRARIES}
  ${NUMPY_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${GFORTRAN_LIBRARIES}
  ${X11_LIBRARIES}
  )

## Installation

install (
  TARGETS ppgplot
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
