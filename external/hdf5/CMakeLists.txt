
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the HDF5
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (HDF5)

## Required version of CMake
cmake_minimum_required (VERSION 2.6)

## Enforced CMake policy 
cmake_policy (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (HDF5_FORCE_BUILD    "Force rebuild and local installation" NO  )
option (HDF5_VERBOSE_BUILD  "Be verbose during build process?"     NO  )
option (HDF5_BUILD_SHARED   "Build shared version of the library?" YES )
option (HDF5_BUILD_STATIC   "Build static version of the library?" YES )
option (HDF5_BUILD_CXX      "Build C++ interface to the library?"  NO  )
option (HDF5_BUILD_HL       "Enable the high level library?"       YES )

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

set (HDF5_FIND_QUIETLY 1)
set (SZIP_FIND_QUIETLY 1)

include (FindHDF5)

## if the there is a system-wide installation, it is the correct version of the
## library? If not we need to force a build from the provided source tree.

## Check for major version
if (HDF5_MAJOR_VERSION LESS 1)
  message (STATUS "HDF5 installation found - major version too small!")
  set (HDF5_FORCE_BUILD TRUE)
else (HDF5_MAJOR_VERSION LESS 1)
  message (STATUS "HDF5 installation found - major version ok.")
  ## Check for major version
  if (HDF5_MINOR_VERSION LESS 8)
    message (STATUS "HDF5 installation found - minor version too small!")
    set (HDF5_FORCE_BUILD TRUE)
  else (HDF5_MINOR_VERSION LESS 8)
    message (STATUS "HDF5 installation found - minor version ok.")
    ## Check for release version
    if (HDF5_RELEASE_VERSION LESS 3)
      message (STATUS "HDF5 installation found - release version too small!")
      set (HDF5_FORCE_BUILD TRUE)
    else (HDF5_RELEASE_VERSION LESS 3)
    message (STATUS "HDF5 installation found - release version ok.")
    endif (HDF5_RELEASE_VERSION LESS 3)
  endif (HDF5_MINOR_VERSION LESS 8)
endif (HDF5_MAJOR_VERSION LESS 1)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Compression/deflation libraries

  include (FindSZIP)
  include (FindZLIB)

  ##__________________________________________________________________
  ## Build list of configuration parameters

  set (HDF5_VERSION "1.8.3")
  set (HDF5_DIRECTORY "hdf5-${HDF5_VERSION}")
  set (HDF5_TAR_ARCHIVE "${HDF5_DIRECTORY}.tar.gz")
  set (HDF5_URL "${USG_DOWNLOAD}/software/${HDF5_TAR_ARCHIVE}")
  
  set (config_settings "")

  if (HDF5_BUILD_CXX)
    set (HDF5_enable_cxx "--enable-cxx") 
  else (HDF5_BUILD_CXX)
    set (HDF5_enable_cxx "--disable-cxx") 
  endif (HDF5_BUILD_CXX)
  
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
  ## 
  
  message (STATUS "[HDF5] Configuration summary.")
  message (STATUS "Library version to be build          = ${HDF5_VERSION}")
  message (STATUS "Build shared version of the library? : ${HDF5_BUILD_SHARED}")
  message (STATUS "Build static version of the library? : ${HDF5_BUILD_STATIC}")
  message (STATUS "Build C++ interface to the library?  : ${HDF5_BUILD_CXX}")
  message (STATUS "Enable the high level library?       : ${HDF5_BUILD_HL}")

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_hdf5_tgz ${HDF5_TAR_ARCHIVE} ${HDF5_SOURCE_DIR})
  
  if (have_hdf5_tgz)
    add_custom_target (download_archive ALL
      COMMAND cp ${have_hdf5_tgz} .
      WORKING_DIRECTORY ${HDF5_BINARY_DIR}
      COMMENT "Copying tar-archive from USG_ROOT/external/hdf5 ..."
      )
  else (have_hdf5_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive ALL
	COMMAND wget -c ${HDF5_URL}
	WORKING_DIRECTORY ${HDF5_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${HDF5_URL} -o ${HDF5_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${HDF5_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[HDF5] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_hdf5_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  if (HDF5_VERBOSE_BUILD)
    add_custom_target (expand_archive ALL
      COMMAND tar -xvzf ${HDF5_TAR_ARCHIVE}
      COMMAND rm ${HDF5_TAR_ARCHIVE}
      WORKING_DIRECTORY ${HDF5_BINARY_DIR}
      COMMENT "Expanding tar-archive ..."
      )
  else (HDF5_VERBOSE_BUILD)
    add_custom_target (expand_archive ALL
      COMMAND tar -xzf ${HDF5_TAR_ARCHIVE}
      COMMAND rm ${HDF5_TAR_ARCHIVE}
      WORKING_DIRECTORY ${HDF5_BINARY_DIR}
      COMMENT "Expanding tar-archive ..."
      )
  endif (HDF5_VERBOSE_BUILD)
  add_dependencies (expand_archive download_archive)
  
  ##__________________________________________________________________
  ## Custom target wrapping the standard configure/build chain
  
  ## Apply required patches
  
  add_custom_target (apply_patches
    COMMAND cp ${HDF5_SOURCE_DIR}/zip_perf.c ${HDF5_DIRECTORY}/perform
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}
    COMMENT "Applying patches to the source code ..."
    )
  add_dependencies (apply_patches expand_archive)

  ## Run "./configure [args]"
  
  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --bindir=${USG_INSTALL_PREFIX}/bin --includedir=${USG_INSTALL_PREFIX}/include/hdf5 ${HDF5_enable_cxx} ${HDF5_enable_hl} ${HDF5_enable_shared} ${HDF5_enable_static}
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Configuring to build HDF5 library ..."
    )
  add_dependencies (run_configure apply_patches)

  ## Run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Start building HDF5 library ..."
    )
  add_dependencies (run_make run_configure)

  ## Run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Installing HDF5 library and header files ..."
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  message (STATUS "Found system-wide installation of HDF5; skipping rebuild!")
endif (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
