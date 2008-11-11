
## Name of the project
project (Pyrap)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (Pyrap_FORCE_BUILD       "Force rebuild and local installation?"    NO )
option (Pyrap_CMAKE_BUILD       "Use CMake to configure and build Pyrap?"  YES )
option (Pyrap_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (Pyrap_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${Pyrap_SOURCE_DIR}
  ${Pyrap_SOURCE_DIR}/..
  ${Pyrap_SOURCE_DIR}/../..
  ${Pyrap_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available



## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYRAP OR Pyrap_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (Pyrap_prefix release_area.txt
    PATHS
    ${Pyrap_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT Pyrap_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT Pyrap_prefix)
  
  ## ------------------------------------------------------------------------------
  ## Required external packages/components

  if (NOT Pyrap_VERBOSE_BUILD)
    set (BOOST_FIND_QUIETLY     YES )
    set (CASACORE_FIND_QUIETLY  YES )
    set (HDF5_FIND_QUIETLY      YES )
    set (PYTHON_FIND_QUIETLY    YES )
  endif (NOT Pyrap_VERBOSE_BUILD)

  include (FindBoost)
  include (FindCASACORE)
  include (FindHDF5)
  include (FindPython)

  if (HAVE_BOOST)
    include_directories (${BOOST_INCLUDES})
  else (HAVE_BOOST)
    message (SEND_ERROR "[pyrap] Missing external package Boost!")
  endif (HAVE_BOOST)
  
  if (HAVE_CASACORE)
    include_directories (${CASACORE_INCLUDES})
  else (HAVE_CASACORE)
    message (SEND_ERROR "[pyrap] Missing external package Casacore!")
  endif (HAVE_CASACORE)
  
  if  (HAVE_HDF5)
    include_directories (${HDF5_INCLUDES})
    add_definitions (-DHAVE_HDF5)
  else (HAVE_HDF5)
    message (STATUS "[pyrap] Support for HDF5 disabled!")
  endif (HAVE_HDF5)

  if (NUMPY_INCLUDES)
    include_directories (${NUMPY_INCLUDES})
    add_definitions (-DAIPS_USENUMPY)
  endif (NUMPY_INCLUDES)
  
  ## ------------------------------------------------------------------------------
  ## Location of header files
  
  include_directories (
    ${Pyrap_SOURCE_DIR}/libpyrap/trunk
    ${PYTHON_INCLUDES}
    )
  
  ## ------------------------------------------------------------------------------
  ## Sub-directories of the project

  add_subdirectory (libpyrap/trunk)

  add_subdirectory (pyrap_fitting/trunk)
  add_subdirectory (pyrap_functionals/trunk)
  add_subdirectory (pyrap_images/trunk)
  add_subdirectory (pyrap_measures/trunk)
  add_subdirectory (pyrap_quanta/trunk)
  add_subdirectory (pyrap_tables/trunk)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYRAP OR Pyrap_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Pyrap; skipping rebuild!")
endif (NOT HAVE_PYRAP OR Pyrap_FORCE_BUILD)
