## ==============================================================================
##
##  Handle the install of the bf2format-related programs for converting bf MS  
##         binary data into other friendly files, like presto.
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(bf2format)

include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc-4.3/ ${PULSAR_SOURCE_DIR}/apps/presto/include/ )

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_C_FLAGS "-O2")
# uncomment when you want to build with -g debugging; comment out the two lines above
#set(CMAKE_CXX_FLAGS "-g -O3")
#set(CMAKE_C_FLAGS "-g -O2")


#the one C file
add_executable(bf2presto bf2presto.c)
target_link_libraries(bf2presto m) #link the math library

add_executable(bf2presto8 bf2presto8.c)
target_link_libraries(bf2presto8 m sigproc presto) #link libs

add_executable(bf2puma2 bf2puma2.cpp)
target_link_libraries(bf2puma2 m) #link the math library

add_executable(bf2fits bf2fits.c)
target_link_libraries(bf2fits m ${CFITSIO_LIBRARIES}) #link the math library

add_executable(2bf2fits 2bf2fits.c)
target_link_libraries(2bf2fits m ${CFITSIO_LIBRARIES}) #link the math library

add_custom_target (bf2format_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto8 ${PULSAR_BINARY_DIR}/apps/bf2format/bf2puma2 ${PULSAR_BINARY_DIR}/apps/bf2format/bf2fits ${PULSAR_BINARY_DIR}/apps/bf2format/2bf2fits ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/bf2format
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/bf2format/bf2presto and bf2presto8 and bf2puma2 and bf2fits and 2bf2fits programs into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )
add_dependencies (bf2format_install bf2presto bf2presto8 bf2puma2 bf2fits 2bf2fits)
#add_dependencies (bf2format_install bf2presto bf2presto8)

install (
  TARGETS bf2presto bf2presto8 bf2puma2 bf2fits 2bf2fits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
