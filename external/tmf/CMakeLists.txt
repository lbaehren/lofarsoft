
## Name of the project
project (TMF)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (TMF_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (TMF_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (TMF_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${TMF_SOURCE_DIR}
  ${TMF_SOURCE_DIR}/..
  ${TMF_SOURCE_DIR}/../..
  ${TMF_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Tmf] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT TMF_VERBOSE_CONFIGURE)
  set (TMF_FIND_QUIETLY YES )
endif (NOT TMF_VERBOSE_CONFIGURE)

include (FindTMF)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_TMF OR TMF_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Tools needed to build modules

  find_program (tar_executable       tar       PATHS ${bin_locations})

  ##__________________________________________________________________
  ## Configure and build the project

  find_file (have_tmf_tgz tmf-1.4.3.tar.gz ${TMF_SOURCE_DIR})
  find_file (have_tmf_cmake tmf_cmake.tar.gz ${TMF_SOURCE_DIR})
  
  if (have_tmf_tgz)
    ExternalProject_Add (tmf
      PREFIX ${TMF_BINARY_DIR}
      URL ${have_tmf_tgz}
      PATCH_COMMAND ${tar_executable} -xzf ${have_tmf_cmake}
      CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${USG_INSTALL_PREFIX}
                 -D USG_ROOT=${USG_ROOT}
      )
  else (have_tmf_tgz)
    message (WARNING "[Tmf] Unable to build project - no sources found!")
  endif (have_tmf_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_TMF OR TMF_FORCE_BUILD)
  add_custom_target (tmf
    WORKING_DIRECTORY ${TMF_BINARY_DIR}
    COMMENT "[Tmf] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_TMF OR TMF_FORCE_BUILD)

