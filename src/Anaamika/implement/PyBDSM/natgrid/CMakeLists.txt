# $Id$

file(GLOB _natgrid_sources Src/*.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

add_library(natgridmodule MODULE ${_natgrid_sources})
set_target_properties(natgridmodule PROPERTIES PREFIX "")

install(TARGETS natgridmodule DESTINATION lib/python/bdsm)
install(FILES   Lib/nat.py    DESTINATION lib/python/bdsm)
