
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

## Package information
set (HDF5_REQUIRED_VERSION_MAJ "1")
set (HDF5_REQUIRED_VERSION_MIN "8")
set (HDF5_REQUIRED_VERSION_REL "5")
set (HDF5_VERSION        "1.${HDF5_REQUIRED_VERSION_MIN}.${HDF5_REQUIRED_VERSION_REL}" )
set (HDF5_SOURCE_ARCHIVE "hdf5-${HDF5_VERSION}.tar.gz" )
set (HDF5_URL            "http://www.hdfgroup.org/ftp/HDF5/prev-releases/hdf5-${HDF5_VERSION}/src/${HDF5_SOURCE_ARCHIVE}" )

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

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${HDF5_SOURCE_DIR}
  ${HDF5_SOURCE_DIR}/..
  ${HDF5_SOURCE_DIR}/../..
  ${HDF5_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_ROOT)


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
if (HDF5_VERSION_MAJOR LESS ${HDF5_REQUIRED_VERSION_MAJ})
  message (STATUS "HDF5 installation found - major version too small!")
  message (STATUS ".. Required ${HDF5_REQUIRED_VERSION_MAJ} - found ${HDF5_VERSION_MAJOR}")
  set (HDF5_FORCE_BUILD TRUE)
else (HDF5_VERSION_MAJOR LESS ${HDF5_REQUIRED_VERSION_MAJ})
  ## Check for major version
  if (HDF5_VERSION_MINOR LESS ${HDF5_REQUIRED_VERSION_MIN})
    message (STATUS "HDF5 installation found - minor version too small!")
    message (STATUS ".. Required ${HDF5_REQUIRED_VERSION_MIN} - found ${HDF5_VERSION_MINOR}")
    set (HDF5_FORCE_BUILD TRUE)
  else (HDF5_VERSION_MINOR LESS ${HDF5_REQUIRED_VERSION_MIN})
    ## Check for release version
    if (HDF5_VERSION_RELEASE LESS ${HDF5_REQUIRED_VERSION_REL})
      message (STATUS "HDF5 installation found - release version too small!")
      message (STATUS ".. Required ${HDF5_REQUIRED_VERSION_REL} - found ${HDF5_VERSION_RELEASE}")
      set (HDF5_FORCE_BUILD TRUE)
    endif (HDF5_VERSION_RELEASE LESS ${HDF5_REQUIRED_VERSION_REL})
  endif (HDF5_VERSION_MINOR LESS ${HDF5_REQUIRED_VERSION_MIN})
endif (HDF5_VERSION_MAJOR LESS ${HDF5_REQUIRED_VERSION_MAJ})

## Run an additional check to see whether we are on a Debian system

if (HDF5_USE_16_API_DEFAULT)
  set (HDF5_FORCE_BUILD TRUE  )
  set (HDF5_FOUND       FALSE )
endif (HDF5_USE_16_API_DEFAULT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  
  include (ExternalProject)
  
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

  ## Locate source file to build from
  
  find_file (HAVE_HDF5_SOURCE ${HDF5_SOURCE_ARCHIVE}
    PATHS ${HDF5_SOURCE_DIR}
    )
  
  if (NOT HAVE_HDF5_SOURCE)
    set (HAVE_HDF5_SOURCE ${HDF5_URL})
  endif (NOT HAVE_HDF5_SOURCE)
  
    ExternalProject_Add (hdf5
      PREFIX ${HDF5_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${HAVE_HDF5_SOURCE}
      CONFIGURE_COMMAND ${HDF5_BINARY_DIR}/source/configure --prefix=${LUS_INSTALL_PREFIX} --bindir=${LUS_INSTALL_PREFIX}/bin --includedir=${LUS_INCLUDE_DIR}/hdf5 ${HDF5_enable_cxx} ${HDF5_enable_fortran} ${HDF5_enable_hl} ${HDF5_enable_shared} ${HDF5_enable_static}
      INSTALL_COMMAND make install
      )
  
  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (hdf5 update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of <package> ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  

  ##__________________________________________________________________
  ## Configuration summary
  
  if (HDF5_VERBOSE_CONFIGURE)
    message (STATUS "------------------------------------------------------------")
    message (STATUS "[HDF5] Configuration summary.")
    message (STATUS "------------------------------------------------------------")
    message (STATUS "Library version")
    message (STATUS ".. System installation               : " ${HDF5_LIBRARY_VERSION} )
    message (STATUS ".. Required library version          : " ${HDF5_VERSION}         )
    message (STATUS "Force rebuild and local installation : " ${HDF5_FORCE_BUILD}     )
    message (STATUS "Be verbose during build process      : " ${HDF5_VERBOSE_BUILD}   )
    message (STATUS "Build shared version of the library  : " ${HDF5_BUILD_SHARED}    )
    message (STATUS "Build static version of the library  : " ${HDF5_BUILD_STATIC}    )
    message (STATUS "Compile the C++ interface            : " ${HDF5_ENABLE_CXX}      )
    message (STATUS "Compile the Fortran interface        : " ${HDF5_ENABLE_FORTRAN}  )
    message (STATUS "Enable the high level library        : " ${HDF5_BUILD_HL}        )
    message (STATUS "Search for MPI support files         : " ${HDF5_ENABLE_PARALLEL} )
    message (STATUS "Enable thread safe capability        : " ${HDF5_ENABLE_THREADSAFE} )
    message ("-------------------------------------------------------------")
  endif (HDF5_VERBOSE_CONFIGURE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  add_custom_target (hdf5
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[HDF5] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
