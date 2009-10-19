
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the installation of casacore from a proviced tar-archive
#
## ------------------------------------------------------------------------------

## Name of the project
project (CASACORE)

## Required version of CMake
cmake_minimum_required (VERSION 2.6)

## Enforced CMake policy 
cmake_policy (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (CASACORE_FORCE_BUILD    "Force rebuild and local installation"  NO  )
option (CASACORE_VERBOSE_BUILD  "Be verbose during build process?"      YES )
option (CASACORE_BUILD_SHARED   "Build shared version of the library?"  NO  )
option (CASACORE_BUILD_STATIC   "Build static version of the library?"  YES )

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
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check for system-wide CASACORE library installation

set (CASACORE_FIND_QUIETLY   YES )
set (HDF5_FIND_QUIETLY       YES )
set (LAPACK_FIND_QUIETLY     YES )

include (FindCASACORE)
include (FindHDF5)
include (FindLAPACK)

## ------------------------------------------------------------------------------
## Update the tar-archive to the latest version of the source

find_program (svn_executable     svn     PATHS ${bin_locations})
find_program (tar_executable     tar     PATHS ${bin_locations})

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

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  
  find_file (have_casacore_tgz   casacore.tgz             ${CASACORE_SOURCE_DIR} )
  find_file (have_cmake_tgz      cmake.tgz                ${CASACORE_SOURCE_DIR} )
  find_file (have_cmakelists_txt casacore/CMakeLists.txt  ${CASACORE_SOURCE_DIR} )

  ##__________________________________________________________________
  ## Build list of configuration parameters

  set (CASACORE_DIRECTORY "casacore")
  set (CASACORE_TAR_ARCHIVE "${CASACORE_DIRECTORY}.tar.gz")
  set (CASACORE_URL "${USG_DOWNLOAD}/software/${CASACORE_TAR_ARCHIVE}")
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  if (CASACORE_VERBOSE_BUILD)
    add_custom_target (expand_archive ALL
      COMMAND ${tar_executable} -xvzf ${have_casacore_tgz}
      COMMAND ${tar_executable} -xvzf ${have_cmake_tgz}
      COMMAND rm -rf build
      COMMAND mkdir -p build
      WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
      COMMENT "Expanding tar-archive ..."
      )
  else (CASACORE_VERBOSE_BUILD)
    add_custom_target (expand_archive ALL
      COMMAND ${tar_executable} -xzf ${have_casacore_tgz}
      COMMAND ${tar_executable} -xzf ${have_cmake_tgz}
      COMMAND rm -rf build
      COMMAND mkdir -p build
      WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
      COMMENT "Expanding tar-archive ..."
      )
  endif (CASACORE_VERBOSE_BUILD)
  
  ##__________________________________________________________________
  ## Custom target wrapping the standard configure/build chain
  
  ## Run CMake
  
  add_custom_target (run_configure ALL
    COMMAND cmake ../casacore
    WORKING_DIRECTORY ${CASACORE_BINARY_DIR}/build
    COMMENT "Configuring to build casacore library ..."
    )
  add_dependencies (run_configure expand_archive)
  
  ## Run "make"
  
  add_custom_target (run_make ALL
    COMMAND make
    COMMAND make install
    WORKING_DIRECTORY ${CASACORE_BINARY_DIR}/build
    COMMENT "Building and installing casacore ..."
    )
  add_dependencies (run_make run_configure)
  
  ## Run "make install"
  
  add_custom_target (install ALL
    COMMAND rm -rf build
    COMMAND rm -rf casacore
    WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
    COMMENT "Cleaning up the build directory ..."
    )
  add_dependencies (install run_make)

  ##__________________________________________________________________
  ## Special targets
  
  ## [1] Update the tar-archive with the CMake scripts
  
  if (tar_executable)
    add_custom_target (update_configuration
      COMMAND ${tar_executable} -cvzf cmake.tgz `find casacore -name CMakeLists.txt`
      COMMAND mv cmake.tgz ${CASACORE_SOURCE_DIR}
      WORKING_DIRECTORY ${CASACORE_BINARY_DIR}
      )
  endif (tar_executable)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  message (STATUS "Found system-wide installation of CASACORE; skipping rebuild!")
endif (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
