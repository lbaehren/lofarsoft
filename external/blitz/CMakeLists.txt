
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the BLITZ
#  library.
#
## ------------------------------------------------------------------------------

project (BLITZ)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BLITZ_FORCE_BUILD "Force rebuild and local installation" 0)
option (BLITZ_CMAKE_BUILD "Use CMake port to configure and build Blitz++?" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BLITZ_SOURCE_DIR}
  ${BLITZ_SOURCE_DIR}/..
  ${BLITZ_SOURCE_DIR}/../..
  ${BLITZ_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## <-- begin build condition --------------------------------------------------->

include (FindBlitz)

if (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)

  # Organization of the source code:
  #
  #  .
  #  |-- benchmarks
  #  |-- bin
  #  |-- blitz
  #  |   |-- array
  #  |   |-- generate
  #  |   |-- gnu
  #  |   `-- meta
  #  |-- config
  #  |-- doc
  #  |   |-- doxygen
  #  |   |-- examples
  #  |   `-- stencils
  #  |-- examples
  #  |-- lib
  #  |-- m4
  #  |-- random
  #  |-- src
  #  `-- testsuite
  
  ## ----------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (BLITZ_prefix release_area.txt
    PATHS
    ${BLITZ_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT BLITZ_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT BLITZ_prefix)

  ## ----------------------------------------------------------------------------
  ## Use CMake to build the Blitz++ library? If this is the case, we wrap the
  ## provided Autotools-based configuration and build
  
  if (BLITZ_CMAKE_BUILD)

    include_directories (
      ${BLITZ_SOURCE_DIR}
      ${BLITZ_SOURCE_DIR}/blitz
      )

    add_definitions (
      -Wno-deprecated
      -fPIC
      )

    add_subdirectory (blitz)

  else (BLITZ_CMAKE_BUILD)
    
    ## [1] configure
    message (STATUS "Running the BLITZ configure script...")
    
    execute_process (
      WORKING_DIRECTORY ${BLITZ_SOURCE_DIR}
      COMMAND ./configure --prefix=${BLITZ_prefix}
      TIMEOUT 300
      ERROR_VARIABLE error_configure
      ERROR_FILE error.log
      )
    
    if (error_configure)
      message (STATUS "There was an error running the BLITZ configure script!")
      message (SEND_ERROR "${error_configure}")
    endif (error_configure)
    
    ## [2] make
    message (STATUS "Building BLITZ library...")
    
    execute_process (
      WORKING_DIRECTORY ${BLITZ_SOURCE_DIR}
      COMMAND make 
      TIMEOUT 3600
      ERROR_VARIABLE error_make
      ERROR_FILE error.log
      )
    
    if (error_make)
      message (STATUS "There was an error building/installing the BLITZ library!")
      message (SEND_ERROR "${error_make}")
    endif (error_make)
    
    ## [3] make install
    
    if (NOT error_make)
      
      message (STATUS "Installing BLITZ library...")
      
      execute_process (
	WORKING_DIRECTORY ${BLITZ_SOURCE_DIR}
	COMMAND make install
	TIMEOUT 180
	ERROR_VARIABLE error_make_install
	ERROR_FILE error.log
	)
      
      if (error_make_install)
	message (STATUS "There was an error building/installing the BLITZ library!")
	message (SEND_ERROR "${error_make_install}")
      endif (error_make_install)
      
    endif (NOT error_make)
    
  endif (BLITZ_CMAKE_BUILD)
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
  message (STATUS "Found system-wide installation of BLITZ; skipping rebuild!")
endif (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
