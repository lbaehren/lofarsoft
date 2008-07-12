##-------------------------------------------------------------------------------
## $Id::                                                                        $
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the BOOST
#  library.
#
#  [./boost/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Name of the project
project (BOOST)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Additional CMake modules

include (CPack)

include (CTest)
enable_testing()

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BOOST_FORCE_BUILD    "Force rebuild and local installation"         NO  )
option (BOOST_CMAKE_BUILD    "Use CMake port to configure and build Boost?" YES )
option (BOOST_ENABLE_TESTING "Enable building and executing test programs?" NO  )

## Selective building of the Boost++ libraries

option (BOOST_LIB_IO          "Build libboost_io"          YES )
option (BOOST_LIB_FILESYSTEM  "Build libboost_filesystem"  YES )
option (BOOST_LIB_LOGIC       "Build libboost_logic"       YES )
option (BOOST_LIB_MATH        "Build libboost_math"        YES )
option (BOOST_LIB_MPL         "Build libboost_mpl"         NO  )
option (BOOST_LIB_PYTHON      "Build libboost_python"      YES )
option (BOOST_LIB_RANGE       "Build libboost_range"       NO  )
option (BOOST_LIB_REGEX       "Build libboost_regex"       NO  )
option (BOOST_LIB_SIGNALS     "Build libboost_signals"     YES )
option (BOOST_LIB_THREAD      "Build libboost_thread"      YES )
option (BOOST_LIB_WAVE        "Build libboost_wave"        YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BOOST_SOURCE_DIR}
  ${BOOST_SOURCE_DIR}/..
  ${BOOST_SOURCE_DIR}/../..
  ${BOOST_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

set ( BOOST_FIND_python_ONLY YES )
set ( BOOST_FIND_QUIETLY     YES )
set ( PYTHON_FIND_QUIETLY    YES )

include (FindBoost)
include (FindPython)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (BOOST_INSTALL_PREFIX release_area.txt
    PATHS
    ${BOOST_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  find_path (BOOST_INSTALL_PREFIX release_area.txt
    PATHS
    ${BOOST_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (BOOST_INSTALL_PREFIX)
    message (STATUS "Installation area located for package BOOST.")
    set (CMAKE_INSTALL_PREFIX ${BOOST_INSTALL_PREFIX}) 
  else (BOOST_INSTALL_PREFIX)
    message (STATUS "Unable to locate installation area for package BOOST.")
  endif (BOOST_INSTALL_PREFIX)
  
  ## ----------------------------------------------------------------------------
  ## CMake port for building Bost

  # (1) Organization of the source code
  #
  #  boost
  #  |-- boost
  #  |   `-- python
  #  |       |-- converter
  #  |       |-- detail
  #  |       |-- object
  #  |       `-- suite
  #  |           `-- indexing
  #  |               `-- detail
  #  `-- libs
  #      `-- python
  #          |-- example
  #          |   |-- quickstart
  #          |   `-- tutorial
  #          |-- pyste
  #          |   |-- dist
  #          |   |-- install
  #          |   |-- src
  #          |   |   `-- Pyste
  #          |   `-- tests
  #          |-- src
  #          |   |-- converter
  #          |   `-- object
  #          `-- test
  #
  # (2) Organization of the header files after installation:
  #
  #  boost
  #  |-- python
  #  |   |-- class.hpp
  #  |   |-- def.hpp
  #  |   |-- extract.hpp
  #  |   |-- implicit.hpp
  #  |   |-- list.hpp
  #  |   |-- module.hpp
  #  |   |-- numeric.hpp
  #  |   |-- object.hpp
  #  |   |-- suite
  #  |   |   `-- indexing
  #  |   |       `-- vector_indexing_suite.hpp
  #  |   `-- tuple.hpp
  #  `-- python.hpp
  
  if (BOOST_CMAKE_BUILD AND HAVE_PYTHON)
    
    message (STATUS "Building Boost using CMake.")

    ## Testing activated?

    if (BOOST_ENABLE_TESTING)
      include (CTest)
      enable_testing()
    endif (BOOST_ENABLE_TESTING)

    ## include paths for header files
    
    include_directories (
      .
      ${BOOST_SOURCE_DIR}
      ${BOOST_SOURCE_DIR}/libs
      ${PYTHON_INCLUDES}
      )

    ## project sub-directories
    
    add_subdirectory (boost)
    add_subdirectory (libs)

    ## summary of configuration settings
    
    message (STATUS " ")
    message (STATUS "============================================================")
    message (STATUS " ")
    message (STATUS "[Boost] Configuration summary")
    message (STATUS "Build libboost_filesystem = ${BOOST_LIB_FILESYSTEM}")
    message (STATUS "Build libboost_io         = ${BOOST_LIB_IO}")
    message (STATUS "Build libboost_mpl        = ${BOOST_LIB_MPL}")
    message (STATUS "Build libboost_python     = ${BOOST_LIB_PYTHON}")
    message (STATUS "Build libboost_range      = ${BOOST_LIB_RANGE}")
    message (STATUS "Build libboost_signals    = ${BOOST_LIB_SIGNALS}")
    message (STATUS "Build libboost_thread     = ${BOOST_LIB_THREAD}")
    message (STATUS "Build libboost_wave       = ${BOOST_LIB_WAVE}")
    message (STATUS " ")
    message (STATUS "============================================================")
    message (STATUS " ")
    
  else (BOOST_CMAKE_BUILD AND HAVE_PYTHON)
    
    ## --------------------------------------------------------------------------
    ## Run the configure script provided along with the distribution
    
    ## [1] configure -------------------------------
    
    ## To be fully consistent at this point, I think we have to ensure that we are
    ## building against the correct python version.
    
    message (STATUS "Running the BOOST configure script...")
    
    if (PYTHON_EXECUTABLE)
      execute_process (
	WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
	COMMAND ./configure --with-libraries=python --with-python=${PYTHON_EXECUTABLE} --prefix=${BOOST_INSTALL_PREFIX}
	TIMEOUT 300
	ERROR_VARIABLE error_configure
	ERROR_FILE error.log
	)
    else (PYTHON_EXECUTABLE)
      execute_process (
	WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
	COMMAND ./configure --with-libraries=python --prefix=${BOOST_INSTALL_PREFIX}
	TIMEOUT 300
	ERROR_VARIABLE error_configure
	ERROR_FILE error.log
	)
    endif (PYTHON_EXECUTABLE)
    
    if (error_configure)
      message (STATUS "There was an error running the BOOST configure script!")
      message (SEND_ERROR "${error_configure}")
    endif (error_configure)
    
    ## [2] make --------------------------------------
    
    message (STATUS "Building BOOST library...")
    
    execute_process (
      WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
      COMMAND make 
      TIMEOUT 3600
      ERROR_VARIABLE error_make
      ERROR_FILE error.log
      )
    
    if (error_make)
      message (STATUS "There was an error building/installing the BOOST library!")
      message (SEND_ERROR "${error_make}")
    endif (error_make)
    
    ## [3] make install ------------------------------
    
    if (NOT error_make)
      
      message (STATUS "Installing BOOST library...")
      
      execute_process (
	WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
	COMMAND make install
	TIMEOUT 180
	ERROR_VARIABLE error_make_install
	ERROR_FILE error.log
	)
      
      if (error_make_install)
	message (STATUS "There was an error building/installing the BOOST library!")
	message (SEND_ERROR "${error_make_install}")
      endif (error_make_install)
      
    endif (NOT error_make)
    
    ## <-- end build condition ------------------------------------------------->
    
  endif (BOOST_CMAKE_BUILD AND HAVE_PYTHON)
  
else (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  message (STATUS "Found system-wide installation of BOOST; skipping rebuild!")
  message (STATUS "BOOST_INCLUDES = ${BOOST_INCLUDES}")
endif (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
