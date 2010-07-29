
## ==============================================================================
##
##  Handle the install of the bf2format-related programs for converting bf MS  
##         binary data into other friendly files, like presto.
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(bf2format)

include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc-4.3/ ${PULSAR_SOURCE_DIR}/apps/presto/include/ )

set(CMAKE_CXX_FLAGS "-g -O3")

#the one C file
add_executable(bf2presto bf2presto.c)
target_link_libraries(bf2presto m) #link the math library

add_executable(bf2presto8 bf2presto8.c)
target_link_libraries(bf2presto8 m sigproc presto) #link libs

add_executable(bf2puma2 bf2puma2.cpp)
target_link_libraries(bf2puma2 m) #link the math library


add_custom_target (bf2format_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto8 ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/bf2format
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto and bf2presto8 and bf2puma2 programs into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )
add_dependencies (bf2format_install bf2presto bf2presto8 bf2puma2)
#add_dependencies (bf2format_install bf2presto bf2presto8)

install (
  TARGETS bf2presto bf2presto8 bf2puma2
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
