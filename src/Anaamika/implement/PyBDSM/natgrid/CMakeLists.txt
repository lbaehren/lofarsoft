# $Id$

file(GLOB _natgrid_sources Src/*.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

add_library(natgridmodule MODULE ${_natgrid_sources})

target_link_libraries (natgridmodule
  ${Boost_PYTHON_LIBRARY}
  ${PYTHON_LIBRARIES}
  )

if (APPLE)
  set_target_properties (natgridmodule
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (natgridmodule
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

install(TARGETS natgridmodule DESTINATION lib/python/bdsm)
install(FILES   Lib/nat.py    DESTINATION lib/python/bdsm)
