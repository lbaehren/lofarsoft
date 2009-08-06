
## Name of the project
project (CFITSIO)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (CFITSIO_FORCE_BUILD       "Force rebuild and local installation?"      NO  )
option (CFITSIO_CMAKE_BUILD       "Use CMake to configure and build CFITSIO?"  YES )
option (CFITSIO_VERBOSE_CONFIGURE "Be verbose during configuration?"           NO  )
option (CFITSIO_VERBOSE_BUILD     "Be verbose during build process?"           NO  )

## Options of the library itself

option (CFITSIO_enable_lfs       "Enable large file system support?" YES )
option (CFITSIO_enable_reentrant "Enable reentrant multithreading?"  NO  )
option (CFITSIO_enable_hera      "Build for HERA project?"           NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${CFITSIO_SOURCE_DIR}
  ${CFITSIO_SOURCE_DIR}/..
  ${CFITSIO_SOURCE_DIR}/../..
  ${CFITSIO_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT CFITSIO_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY  YES )
endif (NOT CFITSIO_VERBOSE_CONFIGURE)

include (FindCFITSIO)

## Check for major version
if (CFITSIO_MAJOR_VERSION LESS 3)
  message (STATUS "CFITSIO installation found - major version too small!")
  set (CFITSIO_FORCE_BUILD TRUE)
else (CFITSIO_MAJOR_VERSION LESS 3)
  ## Check for major version
  if (CFITSIO_MINOR_VERSION LESS 180)
    message (STATUS "CFITSIO installation found - minor version too small!")
    set (CFITSIO_FORCE_BUILD TRUE)
  else (CFITSIO_MINOR_VERSION LESS 180)
  endif (CFITSIO_MINOR_VERSION LESS 180)
endif (CFITSIO_MAJOR_VERSION LESS 3)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## 

  set (CFITSIO_VERSION "3181")
  set (CFITSIO_DIRECTORY "cfitsio${CFITSIO_VERSION}")
  set (CFITSIO_TAR_ARCHIVE "${CFITSIO_DIRECTORY}.tar.gz")
  
  find_file (have_cfitsio_tgz ${CFITSIO_TAR_ARCHIVE} ${CFITSIO_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})

  if (have_cfitsio_tgz AND tar_executable)
    
    ## expand the tar-archive
    add_custom_target (cfitsio_expand
      COMMAND ${tar_executable} -xvzf ${have_cfitsio_tgz}
      WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}
      COMMENT "[CFITSIO] Expanding tar-archive ..."
      )
    
    ## configure the package
    if (CFITSIO_enable_lfs)
      if (CFITSIO_enable_reentrant)
	add_custom_target (cfitsio_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --includedir=${USG_INSTALL_PREFIX}/include/cfitsio --enable-lfs --enable-reentrant
	  WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
	  COMMENT "[CFITSIO] Configuring package ..."
	  )
      else (CFITSIO_enable_reentrant)
	add_custom_target (cfitsio_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --includedir=${USG_INSTALL_PREFIX}/include/cfitsio --enable-lfs
	  WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
	  COMMENT "[CFITSIO] Configuring package ..."
	  )
      endif (CFITSIO_enable_reentrant)
    else (CFITSIO_enable_lfs)
      if (CFITSIO_enable_reentrant)
	add_custom_target (cfitsio_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --includedir=${USG_INSTALL_PREFIX}/include/cfitsio --enable-reentrant
	  WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
	  COMMENT "[CFITSIO] Configuring package ..."
	  )
      else (CFITSIO_enable_reentrant)
	add_custom_target (cfitsio_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --includedir=${USG_INSTALL_PREFIX}/include/cfitsio 
	  WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
	  COMMENT "[CFITSIO] Configuring package ..."
	  )
      endif (CFITSIO_enable_reentrant)
    endif (CFITSIO_enable_lfs)
    ## target dependency
    add_dependencies (cfitsio_configure cfitsio_expand)
    
    ## build the package
    add_custom_target (cfitsio_build
      COMMAND make
      WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
      COMMENT "[CFITSIO] Starting build ..."
      )
    add_dependencies (cfitsio_build cfitsio_configure)
    
    ## build and install the package
    add_custom_target (cfitsio_install
      COMMAND mkdir -p ${USG_INSTALL_PREFIX}/include/cfitsio
      COMMAND make install
      COMMAND mv ${USG_INSTALL_PREFIX}/include/*.h ${USG_INSTALL_PREFIX}/include/cfitsio
      WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}/cfitsio
      COMMENT "[CFITSIO] Installing package ..."
      )
    add_dependencies (cfitsio_install cfitsio_build)
    
    ## post-install clean-up
    add_custom_target (install
      COMMAND rm -rf *
      WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}
      COMMENT "[CFITSIO] Cleaning up build directory ..."
      )
    add_dependencies (install cfitsio_install)
    
  else (have_cfitsio_tgz AND tar_executable)
    message (STATUS "[CFITSIO] Unable to install - missing tar-archive!")
  endif (have_cfitsio_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)
  message (STATUS "Found system-wide installation of CFITSIO; skipping rebuild!")
endif (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Configuration summary

message ("--------------------------------------------------")
message ("   [CFITSIO] Configuration summary.")
message (" ")
message (STATUS "CFITSIO_LIBRARIES = ${CFITSIO_LIBRARIES}")
message (STATUS "enable HERA       = ${CFITSIO_enable_hera}")
message (STATUS "enable_reentrant  = ${CFITSIO_enable_reentrant}")
message ("--------------------------------------------------")
