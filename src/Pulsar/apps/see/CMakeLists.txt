
## ==============================================================================
##
##  Handle the install binary files viewer 'see'
##
## ==============================================================================

cmake_minimum_required(VERSION 2.6)

project(see)

add_executable (see
  see.cpp
  regard.cpp
  mem.cpp
  util.cpp
  gifsave.cpp
  )

set_target_properties (see
  PROPERTIES
  COMPILE_FLAGS "-c -g -O3 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -Wno-deprecated -I./ -I/usr/X11R6/include -I${PULSAR_SOURCE_DIR}/apps/see"
  LINK_FLAGS "-lX11 -lXt -lSM -lICE -L${X11_Xt_LIB}"
#  target_link_libraries (see X11 Xt SM ICE)
#  target_link_libraries (see -lXt ${X11_Xt_LIB})
  )

add_custom_target (see_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/see/see ${CMAKE_INSTALL_PREFIX}/bin
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/see
  COMMENT "Install ${PULSAR_BINARY_DIR}/apps/see/see program into ${CMAKE_INSTALL_PREFIX}/bin/..."
  )

add_dependencies (see_install see)

