
## Name of the project
project (BOOST)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BOOST_FORCE_BUILD       "Force rebuild and local installation?"  YES )
option (BOOST_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (BOOST_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BOOST_SOURCE_DIR}
  ${BOOST_SOURCE_DIR}/..
  ${BOOST_SOURCE_DIR}/../..
  ${BOOST_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Boost++] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (BOOST_FIND_QUIETLY YES )

include (FindBoost)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)
  find_program (tar_executable tar ${bin_locations})
  find_program (ln_executable  ln  ${bin_locations})

  ##__________________________________________________________________
  ## Configure and build the project

  set (boost_version "1_40_0")
  set (boost_preprocessor ${USG_ROOT}/release/include/boost/preprocessor)

  find_file (have_boost_tgz   boost_${boost_version}.tar.gz ${BOOST_SOURCE_DIR})
  find_file (have_boost_cmake boost_cmake.tgz               ${BOOST_SOURCE_DIR})
  
  configure_file (
    ${BOOST_SOURCE_DIR}/boost.cmake.in
    ${BOOST_BINARY_DIR}/boost.cmake
    )

  if (have_boost_tgz)
    ## Basic install of the package
    ExternalProject_Add (boost
      PREFIX ${BOOST_BINARY_DIR}
      DOWNLOAD_DIR ${BOOST_BINARY_DIR}/boost
      PATCH_COMMAND ${tar_executable} -xzf ${have_boost_cmake}
      SOURCE_DIR ${BOOST_BINARY_DIR}/boost
      URL ${have_boost_tgz}
      CMAKE_ARGS -C ${BOOST_BINARY_DIR}/boost.cmake
      )
    ## Post-installation patch of include paths
    ExternalProject_Add_Step (boost boost_step1
      COMMAND rm -f ${boost_preprocessor}/seq/detail
      COMMAND ${ln_executable} -s ${boost_preprocessor}/detail ${boost_preprocessor}/seq/detail
      COMMENT "Creating symbolic link seq/detail -> detail"
      DEPENDEES install
      WORKING_DIRECTORY ${boost_preprocessor}
      )
  else (have_boost_tgz)
    message (WARNING "[Boost++] Unable to build project - no sources found!")
  endif (have_boost_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  add_custom_target (boost
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Boost++] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Boost++] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
