
## ==============================================================================
##
##  Handle the install binary files viewer 'see'
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(see)

# building the program only for Linux/UNIX architectures
if (NOT APPLE)

add_executable (see
  see.cpp
  regard.cpp
  mem.cpp
  util.cpp
  gifsave.cpp
  )

add_custom_target (see_config
 COMMAND sh ${PULSAR_SOURCE_DIR}/apps/see/config.sh
 WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/see
 COMMENT "Configuring see ..." 
 )

set_target_properties (see
  PROPERTIES
  COMPILE_FLAGS "-c -g -O3 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -Wno-deprecated -I./ -I${X11_X11_INCLUDE_PATH} -I${PULSAR_SOURCE_DIR}/apps/see -I${PULSAR_BINARY_DIR}/apps/see"
  LINK_FLAGS "-L${X11_X11_LIB} -lXt -lSM -lICE" 
#  target_link_libraries (see X11 Xt SM ICE)
#  target_link_libraries (see -lXt ${X11_Xt_LIB})
  )

add_dependencies (see see_config)

add_custom_target (see_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/see/see ${CMAKE_INSTALL_PREFIX}/bin
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/see
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/see/see program into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )

add_dependencies (see_install see)

else (NOT APPLE)

add_custom_target (see
 COMMENT "APPLE platform - skipping 'see' installation ..." 
 )

endif (NOT APPLE)
