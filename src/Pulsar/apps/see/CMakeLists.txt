
## Name of the project
project(see)

## Minimum required version of CMake to configure the project
cmake_minimum_required(VERSION 2.6)

## ==============================================================================
##
##  Configure file
##
##  Any occurrences of #cmakedefine VAR will be replaced with either #define VAR
##  or /* #undef VAR */ depending on the setting of VAR in CMake. Any occurrences
##  of #cmakedefine01 VAR will be replaced with either #define VAR 1 or
##  #define VAR 0 depending on whether VAR evaluates to TRUE or FALSE in CMake
##
## ==============================================================================

## System inspection 

find_file (HAVE_SYS_TIME_H       sys/time.h      )
find_file (HAVE_SYS_UTSNAME_H    sys/utsname.h   )
find_file (HAVE_SYS_SYSINFO_H    sys/sysinfo.h   )
find_file (HAVE_LINUX_VERSION_H  linux/version.h )

## Generate configure file

configure_file (
  ${PROJECT_SOURCE_DIR}/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
)

## ==============================================================================
##
##  Handle the install binary files viewer 'see'
##
## ==============================================================================

# building the program only for Linux/UNIX architectures

if (NOT APPLE)
  
  include_directories (
    ${X11_X11_INCLUDE_PATH}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    )
  
  add_executable (see
    see.cpp
    regard.cpp
    mem.cpp
    util.cpp
    gifsave.cpp
    )
  
  set_target_properties (see
    PROPERTIES
    COMPILE_FLAGS "-c -g -O3 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -Wno-deprecated"
    LINK_FLAGS "-L${X11_X11_LIB} -lXt -lSM -lICE" 
    #  target_link_libraries (see X11 Xt SM ICE)
    #  target_link_libraries (see -lXt ${X11_Xt_LIB})
    )
  
  add_dependencies (see see_config)
  
  add_custom_target (see_install
    COMMAND cp ${PROJECT_BINARY_DIR}/see ${CMAKE_INSTALL_PREFIX}/bin
    WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/see
    COMMENT "Install ${PROJECT_BINARY_DIR}/see program into ${CMAKE_INSTALL_PREFIX}/bin/..."
    )
  
  add_dependencies (see_install see)
  
else (NOT APPLE)
  
  add_custom_target (see
    COMMENT "APPLE platform - skipping 'see' installation ..." 
    )
  
endif (NOT APPLE)
