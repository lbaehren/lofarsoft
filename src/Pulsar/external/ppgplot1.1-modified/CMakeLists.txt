
## compiler instructions
remove_definitions (-dynamiclib -install_name)

add_library (ppgplot SHARED
  ppgplot.c
  )

target_link_libraries (ppgplot
  ${PGPLOT_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${NUMPY_LIBRARIES}
  ${NUMERIC_LIBRARIES}
  ${GFORTRAN_LIBRARIES}
  ${X11_LIBRARIES}
  )

## additional target properties
if (APPLE)
  set_target_properties (ppgplot
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-bundle -DNDEBUG -g"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (ppgplot
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DNDEBUG -g"
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

## Installation

install (
  TARGETS ppgplot
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
