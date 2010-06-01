
## ==============================================================================
##
##  Handle the install of the fluxmaster program to calculate the s/n and flux 
##  of a pulsar based on the pulse profile that presto generates (*.bestprof)
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(fluxmaster)

#include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc-4.3/ ${PULSAR_SOURCE_DIR}/apps/fluxmaster )

set(CMAKE_CXX_FLAGS "-g -O3")

#the one C++ file
add_executable(fluxmaster fluxmaster.cpp)
target_link_libraries(fluxmaster ${PGPLOT_LIBRARIES}) 

add_custom_target (fluxmaster_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/fluxmaster/fluxmaster ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/fluxmaster
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/fluxmaster/fluxmaster program into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )
add_dependencies (fluxmaster_install fluxmaster)

install (
  TARGETS fluxmaster
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
