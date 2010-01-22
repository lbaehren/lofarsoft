
## Name of the project
project (CASACORE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (CASACORE_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (CASACORE_CMAKE_BUILD       "Use CMake to configure and build CASACORE?"  YES )
option (CASACORE_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (CASACORE_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${CASACORE_SOURCE_DIR}
  ${CASACORE_SOURCE_DIR}/..
  ${CASACORE_SOURCE_DIR}/../..
  ${CASACORE_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[CASACORE] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (CASACORE_FIND_QUIETLY YES)

include (FindCASACORE)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  
  include (ExternalProject)
  
  find_file (have_casacore_tgz casacore.tgz  ${CASACORE_SOURCE_DIR})
  find_program (tar_executable tar PATHS ${bin_locations})

  if (have_casacore_tgz)
    ExternalProject_Add (casacore
      PREFIX ${CASACORE_BINARY_DIR}
      DOWNLOAD_DIR ${CASACORE_BINARY_DIR}
      SOURCE_DIR ${CASACORE_BINARY_DIR}
      URL ${have_casacore_tgz}
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${USG_INSTALL_PREFIX}
      )
  else (have_casacore_tgz)
    ExternalProject_Add (casacore
      PREFIX ${CASACORE_BINARY_DIR}
      DOWNLOAD_DIR ${CASACORE_BINARY_DIR}
      SOURCE_DIR ${CASACORE_BINARY_DIR}
      URL http://casacore.googlecode.com/svn/trunk casacore
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${USG_INSTALL_PREFIX}
      )
  endif (have_casacore_tgz)

  ## Put CMake configuration scripts into place
  ExternalProject_Add_Step (casacore casacore_cmake
    COMMAND tar -xvzf ${CASACORE_SOURCE_DIR}/cmake.tgz
    COMMENT "Creating symbolic links for include directories"
    DEPENDEES patch
    DEPENDERS configure
    WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
    )
  
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  add_custom_target (casacore
    WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
    COMMENT "[CASACORE] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Update the tar-archive to the latest version of the source

find_program (svn_executable  svn  PATHS ${bin_locations})
find_program (tar_executable  tar  PATHS ${bin_locations})

if (svn_executable AND tar_executable)
  add_custom_target (update_casacore
    COMMAND rm -rf casacore
    COMMAND ${svn_executable} co http://casacore.googlecode.com/svn/trunk casacore
    COMMAND ${tar_executable} -cvzf casacore.tgz casacore --exclude .svn
    COMMAND rm -rf casacore
    WORKING_DIRECTORY ${CASACORE_SOURCE_DIR}
    COMMENT "Retrieving latest version of casacore source code ..."
    )
endif (svn_executable AND tar_executable)  

