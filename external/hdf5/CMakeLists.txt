
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the HDF5
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (HDF5)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (HDF5_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (HDF5_VERBOSE_BUILD     "Be verbose during build process?"          NO  )
option (HDF5_BUILD_SHARED      "Build shared version of the library?"      YES )
option (HDF5_BUILD_STATIC      "Build static version of the library?"      YES )
option (HDF5_ENABLE_CXX        "Compile the C++ interface?"                YES )
option (HDF5_ENABLE_FORTRAN    "Compile the Fortran interface?"            NO  )
option (HDF5_BUILD_HL          "Enable the high level library?"            YES )
option (HDF5_ENABLE_PARALLEL   "Search for MPI-IO and MPI support files?"  NO  )
option (HDF5_ENABLE_THREADSAFE "Enable thread safe capability?"            NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${HDF5_SOURCE_DIR}
  ${HDF5_SOURCE_DIR}/..
  ${HDF5_SOURCE_DIR}/../..
  ${HDF5_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)


## ------------------------------------------------------------------------------
## Check for system-wide HDF5 library installation

set (HDF5_FIND_QUIETLY YES)
set (SZIP_FIND_QUIETLY YES)
set (ZLIB_FIND_QUIETLY YES)

include (FindHDF5)
include (FindSZIP)
include (FindZLIB)

## if the there is a system-wide installation, it is the correct version of the
## library? If not we need to force a build from the provided source tree.

## Check for major version
if (HDF5_MAJOR_VERSION LESS 1)
  message (STATUS "HDF5 installation found - major version too small!")
  set (HDF5_FORCE_BUILD TRUE)
else (HDF5_MAJOR_VERSION LESS 1)
  ## Check for major version
  if (HDF5_MINOR_VERSION LESS 8)
    message (STATUS "HDF5 installation found - minor version too small!")
    set (HDF5_FORCE_BUILD TRUE)
  else (HDF5_MINOR_VERSION LESS 8)
    ## Check for release version
    if (HDF5_RELEASE_VERSION LESS 3)
      message (STATUS "HDF5 installation found - release version too small!")
      set (HDF5_FORCE_BUILD TRUE)
    endif (HDF5_RELEASE_VERSION LESS 3)
  endif (HDF5_MINOR_VERSION LESS 8)
endif (HDF5_MAJOR_VERSION LESS 1)

## Run an additional check to see whether we are on a Debian system

## [...]

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  
  include (ExternalProject)

  set (HDF5_VERSION     "1.8.3")
  set (HDF5_DIRECTORY   "hdf5-${HDF5_VERSION}")
  set (HDF5_TAR_ARCHIVE "${HDF5_DIRECTORY}.tar.gz")
  set (HDF5_URL "${USG_DOWNLOAD}/software/${HDF5_TAR_ARCHIVE}")
  
  ##__________________________________________________________________
  ## Configuration parameters

  if (HDF5_ENABLE_CXX)
    set (HDF5_enable_cxx "--enable-cxx") 
  else (HDF5_ENABLE_CXX)
    set (HDF5_enable_cxx "--disable-cxx") 
  endif (HDF5_ENABLE_CXX)
  
  if (HDF5_ENABLE_FORTRAN)
    set (HDF5_enable_fortran "--enable-fortran") 
  else (HDF5_ENABLE_FORTRAN)
    set (HDF5_enable_fortran "--disable-fortran") 
  endif (HDF5_ENABLE_FORTRAN)
  
  if (HDF5_BUILD_HL)
    set (HDF5_enable_hl "--enable-hl") 
  else (HDF5_BUILD_HL)
    set (HDF5_enable_hl "--disable-hl") 
  endif (HDF5_BUILD_HL)

  if (HDF5_BUILD_SHARED)
    set (HDF5_enable_shared "--enable-shared") 
  else (HDF5_BUILD_SHARED)
    set (HDF5_enable_shared "--disable-shared") 
  endif (HDF5_BUILD_SHARED)
  
  if (HDF5_BUILD_STATIC)
    set (HDF5_enable_static "--enable-static") 
  else (HDF5_BUILD_STATIC)
    set (HDF5_enable_static "--disable-static") 
  endif (HDF5_BUILD_STATIC)

  ##__________________________________________________________________
  ## Build the project

  find_file (have_hdf5_tgz ${HDF5_TAR_ARCHIVE} ${HDF5_SOURCE_DIR})

  if (have_hdf5_tgz)
    ExternalProject_Add (hdf5
      PREFIX ${HDF5_BINARY_DIR}
      DOWNLOAD_DIR ${HDF5_BINARY_DIR}
      SOURCE_DIR ${HDF5_BINARY_DIR}
      URL ${have_hdf5_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${HDF5_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX} --bindir=${USG_INSTALL_PREFIX}/bin --includedir=${USG_INSTALL_PREFIX}/include/hdf5 ${HDF5_enable_cxx} ${HDF5_enable_fortran} ${HDF5_enable_hl} ${HDF5_enable_shared} ${HDF5_enable_static}
      )
  else (have_hdf5_tgz)
    ExternalProject_Add (hdf5
      PREFIX ${HDF5_BINARY_DIR}
      DOWNLOAD_DIR ${HDF5_BINARY_DIR}
      SOURCE_DIR ${HDF5_BINARY_DIR}
      URL ${HDF5_URL}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${HDF5_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX} --bindir=${USG_INSTALL_PREFIX}/bin --includedir=${USG_INSTALL_PREFIX}/include/hdf5 ${HDF5_enable_cxx} ${HDF5_enable_fortran} ${HDF5_enable_hl} ${HDF5_enable_shared} ${HDF5_enable_static}
      )
  endif (have_hdf5_tgz)
  
  ##__________________________________________________________________
  ## Custom target wrapping the standard configure/build chain
  
#  add_custom_target (apply_patches
#    COMMAND cp ${HDF5_SOURCE_DIR}/zip_perf.c ${HDF5_DIRECTORY}/perform
#    WORKING_DIRECTORY ${HDF5_BINARY_DIR}
#    COMMENT "Applying patches to the source code ..."
#    )

  ##__________________________________________________________________
  ## Configuration summary

  message ("-------------------------------------------------------------")
  message (STATUS "Force rebuild and local installation : " ${HDF5_FORCE_BUILD}    )
  message (STATUS "Be verbose during build process      : " ${HDF5_VERBOSE_BUILD}  )
  message (STATUS "Build shared version of the library  : " ${HDF5_BUILD_SHARED}   )
  message (STATUS "Build static version of the library  : " ${HDF5_BUILD_STATIC}   )
  message (STATUS "Compile the C++ interface            : " ${HDF5_ENABLE_CXX}     )
  message (STATUS "Compile the Fortran interface        : " ${HDF5_ENABLE_FORTRAN} )
  message (STATUS "Enable the high level library        : " ${HDF5_BUILD_HL}       )
  message (STATUS "Search for MPI support files         : " ${HDF5_ENABLE_PARALLEL} )
  message (STATUS "Enable thread safe capability        : " ${HDF5_ENABLE_THREADSAFE} )
  message ("-------------------------------------------------------------")

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  add_custom_target (hdf5
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[HDF5] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
