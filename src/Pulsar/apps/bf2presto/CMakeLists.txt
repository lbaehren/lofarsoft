
## ==============================================================================
##
##  Handle the install of the bf2presto program for converting bf MS binary data 
##         into presto-friendly files
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(bf2presto)

set(CMAKE_CXX_FLAGS "-g -O3")

#the one C file
add_executable(bf2presto bf2presto.c)
target_link_libraries(bf2presto m) #link the math library

add_executable(bf2presto8 bf2presto8.c)
target_link_libraries(bf2presto8 m) #link the math library


add_custom_target (bf2presto_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/bf2presto/bf2presto ${PULSAR_BINARY_DIR}/apps/bf2presto/bf2presto8 ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/bf2presto
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/bf2presto/bf2presto and bf2presto8 program into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )
add_dependencies (bf2presto_install bf2presto bf2presto8)
