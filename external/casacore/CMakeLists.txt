
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (CASACORE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

##  Package information
set (CASACORE_VERSION        "1.2.0" )
set (CASACORE_SOURCE_ARCHIVE "casacore-${CASACORE_VERSION}.tar.bz2" )
set (CASACORE_URL            "http://code.google.com/p/casacore/downloads/detail?name=${CASACORE_SOURCE_ARCHIVE}" )
set (CASACORE_SVN            "http://casacore.googlecode.com/svn/trunk" )

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (CASACORE_FORCE_BUILD    "Force build and installation of package?" NO  )
option (CASACORE_FROM_REPOS     "Install package from repository?"         YES )
option (CASACORE_ENABLE_TESTING "Enable testing and experimental builds?"  YES )
option (CASACORE_WITH_FFTW3     "Enable FFTW3 library support?"            NO  )
option (CASACORE_WITH_HDF5      "Enable HDF5 library support?"             YES )
option (CASACORE_BUILD_TESTING  "Build test programs?"                     NO  )

## Handle options

if (CASACORE_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (CASACORE_ENABLE_TESTING)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

##__________________________________________________________
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/..
  ${PROJECT_SOURCE_DIR}/../..
  ${PROJECT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  ## Global CMake LUS settings
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  ## Load find modules
  include (FindCFITSIO)
  include (FindGFortran)
  include (FindLAPACK)
  include (FindWCSLIB)
else (LUS_ROOT)
  message (FATAL_ERROR "[casacore] Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

if (GFORTRAN_LIBRARY)
  list (APPEND LAPACK_LIBRARIES ${GFORTRAN_LIBRARY})
endif (GFORTRAN_LIBRARY)

##__________________________________________________________
## Handle option: HDF5 library support

if (CASACORE_WITH_HDF5)
  include (FindHDF5)
  include (FindZLIB)

  if (ZLIB_LIBRARIES)
    list (APPEND HDF5_LIBRARIES ${ZLIB_LIBRARIES})
  endif (ZLIB_LIBRARIES)
endif (CASACORE_WITH_HDF5)

##__________________________________________________________
## Handle option: FFTW3 library support

if (CASACORE_WITH_FFTW3)
  include (FindFFTW3)

  if (NOT FFTW3_FOUND)
    set (CASACORE_WITH_FFTW3 FALSE)
  endif (NOT FFTW3_FOUND)
endif (CASACORE_WITH_FFTW3)

##__________________________________________________________
## Create configure file for initial cache

configure_file (
  ${PROJECT_SOURCE_DIR}/casacore.cmake.in
  ${PROJECT_BINARY_DIR}/casacore.cmake)

find_file (CASACORE_CMAKE casacore.cmake
  PATHS ${PROJECT_BINARY_DIR}
  )

if (NOT CASACORE_CMAKE)
  message (SEND_ERROR "[casacore] Failed to locate initial cache values!")
endif (NOT CASACORE_CMAKE)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (CASACORE_FROM_REPOS)
  set (CASACORE_FORCE_BUILD TRUE)
endif (CASACORE_FROM_REPOS)

if (NOT CASACORE_FOUND OR CASACORE_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (CASACORE_SOURCE ${CASACORE_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT CASACORE_SOURCE)
    set (CASACORE_SOURCE ${CASACORE_URL})
  endif (NOT CASACORE_SOURCE)
  
  ## Build package from source
  
  if (CASACORE_FROM_REPOS)
    
    ExternalProject_Add (casacore
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${CASACORE_SVN}
      CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
      CMAKE_AGRS -C${CASACORE_CMAKE}
      BUILD_COMMAND make -j5
      INSTALL_COMMAND sudo make install
      )
  
  else (CASACORE_FROM_REPOS)
    
    ExternalProject_Add (casacore
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${CASACORE_SOURCE}
      CMAKE_CACHE_ARGS _DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
      CMAKE_AGRS -C${PROJECT_BINARY_DIR}/casacore.cmake
      INSTALL_COMMAND sudo make install
      )
    
  endif (CASACORE_FROM_REPOS)
  
else (NOT CASACORE_FOUND OR CASACORE_FORCE_BUILD)
  
  add_custom_target (casacore
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[CASACORE] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT CASACORE_FOUND OR CASACORE_FORCE_BUILD)
