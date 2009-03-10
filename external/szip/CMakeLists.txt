
## Name of the project
project (SZIP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SZIP_SOURCE_DIR}
  ${SZIP_SOURCE_DIR}/..
  ${SZIP_SOURCE_DIR}/../..
  ${SZIP_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
##  Configuration/Build options

option (SZIP_FORCE_BUILD    "Force rebuild and local installation"  NO  )
option (SZIP_DEBUG          "Enable debugging information?"         NO  )
option (SZIP_ENABLE_TESTING "Enable building and running tests?"    YES )

## ------------------------------------------------------------------------------
## Check for possible existing version

set (SZIP_FIND_QUIETLY YES)

include (FindSZIP)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
  
  ## ---------------------------------------------------------------------------
  ## System header files
  
  include (CheckTypeSize)

  find_path (HAVE_DLFCN_H      dlfcn.h      PATHS ${include_locations} )
  find_path (HAVE_ERRNO_H      errno.h      PATHS ${include_locations} )
  find_path (HAVE_FCNTL_H      fcntl.h      PATHS ${include_locations} )
  find_path (HAVE_INTTYPES_H   inttypes.h   PATHS ${include_locations} )
  find_path (HAVE_MEMORY_H     memory.h     PATHS ${include_locations} )
  find_path (HAVE_STRING_H     string.h     PATHS ${include_locations} )
  find_path (HAVE_STRINGS_H    strings.h    PATHS ${include_locations} )
  find_path (HAVE_STDDEF_H     stddef.h     PATHS ${include_locations} )
  find_path (HAVE_STDINT_H     stdint.h     PATHS ${include_locations} )
  find_path (HAVE_STDLIB_H     stdlib.h     PATHS ${include_locations} )
  find_path (HAVE_UNISTD_H     unistd.h     PATHS ${include_locations} )
  find_path (HAVE_SYS_MMAN_H   sys/mman.h   PATHS ${include_locations} )
  find_path (HAVE_SYS_STAT_H   sys/stat.h   PATHS ${include_locations} )
  find_path (HAVE_SYS_TYPES_H  sys/types.h  PATHS ${include_locations} )
  
  check_type_size ("unsigned int" size_t)
  check_type_size ("int"          HAVE_INT)
  check_type_size ("long"         HAVE_LONG)
  check_type_size ("long long"    HAVE_LONGLONG)
  
  ## ---------------------------------------------------------------------------
  ## Create SZconfig.h
  
  configure_file (
    ${USG_ROOT}/devel_common/cmake/config_szip.h.in
    ${SZIP_BINARY_DIR}/SZconfig.h
    )
  
  ## ----------------------------------------------------------------------------
  ##  Build the library
  
  include_directories (
    ${SZIP_SOURCE_DIR}/src
    ${SZIP_BINARY_DIR}
    )
  
  set (szip_sources
    src/encoding.c
    src/rice.c
    src/sz_api.c
    )

  set (szip_headers
    src/rice.h
    src/ricehdf.h
    src/szip_adpt.h
    src/szlib.h
    )
  
  add_library (szip ${szip_sources})
  
  ## ----------------------------------------------------------------------------
  ##  Installation
  
  find_path (SZIP_INSTALL_PREFIX release_area.txt
    PATHS
    ${SZIP_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (SZIP_INSTALL_PREFIX)
    message (STATUS "Installation area located for package SZIP.")
    set (CMAKE_INSTALL_PREFIX ${SZIP_INSTALL_PREFIX}) 
  endif (SZIP_INSTALL_PREFIX)

  ## Installation of the library
  
  install (TARGETS szip
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
  
  ## Installation of header files
  
  install (FILES ${szip_headers}
    DESTINATION include/szip
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
  message (STATUS "Found system-wide installation of SZIP; skipping rebuild!")
  message (STATUS "SZIP_INCLUDES  = ${SZIP_INCLUDES}")
  message (STATUS "SZIP_LIBRARIES = ${SZIP_LIBRARIES}")
endif (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
