
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

option (HDF5_FORCE_BUILD  "Force rebuild and local installation" NO  )
option (HDF5_BUILD_SHARED "Build shared version of the library?" YES )
option (HDF5_BUILD_STATIC "Build static version of the library?" YES )
option (HDF5_BUILD_CXX    "Build C++ interface to the library?"  NO  )
option (HDF5_BUILD_HL     "Enable the high level library?"       YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${HDF5_SOURCE_DIR}
  ${HDF5_SOURCE_DIR}/..
  ${HDF5_SOURCE_DIR}/../..
  ${HDF5_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[HDF5] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check for system-wide HDF5 library installation

set (HDF5_FIND_QUIETLY 1)
set (SZIP_FIND_QUIETLY 1)

include (FindHDF5)

## if the there is a system-wide installation, it is the correct version of the
## library? If not we need to force a build from the provided source tree.

if (HDF5_MAJOR_VERSION LESS 1 OR HDF5_MINOR_VERSION LESS 8)
  message (STATUS "System-wide installation of HDF5 library found - but too old!")
  set (HDF5_FORCE_BUILD TRUE)
endif (HDF5_MAJOR_VERSION LESS 1 OR HDF5_MINOR_VERSION LESS 8)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (HDF5_prefix release_area.txt
    PATHS
    ${HDF5_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT HDF5_prefix)
    message (FATAL_ERROR "[HDF5] Unable to locate the installation area")
  endif (NOT HDF5_prefix)
  
  ## ----------------------------------------------------------------------------
  ## Compression/deflation libraries

  include (FindSZIP)
  include (FindZLIB)

  ##__________________________________________________________________
  ## Build list of configuration parameters

  set (HDF5_VERSION "1.8.2")
  set (HDF5_TAR_ARCHIVE "hdf5-${HDF5_VERSION}.tar.gz")
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
	  COMMAND curl ${HDF5_URL} -o ${HDF5_TAR_ARCHIVE} -C
	  WORKING_DIRECTORY ${HDF5_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[HDF5] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_hdf5_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive ALL
    COMMAND tar -xvzf ${HDF5_TAR_ARCHIVE}
    COMMAND rm ${HDF5_TAR_ARCHIVE}
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)
  
  ##__________________________________________________________________
  ## Custom target wrapping the standard configure/build chain

  ## Run "./configure [args]"
  
  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${HDF5_prefix} --bindir=${HDF5_prefix}/bin --includedir=${HDF5_prefix}/include/hdf5 ${HDF5_enable_cxx} ${HDF5_enable_hl} ${HDF5_enable_shared} ${HDF5_enable_static}
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Configuring to build HDF5 library ..."
    )
  add_dependencies (run_configure expand_archive)

  ## Run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Start building HDF5 library ..."
    )
  add_dependencies (run_make run_configure)

  ## Run "make install"

  add_custom_target (run_install ALL
    COMMAND make install
    WORKING_DIRECTORY ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    COMMENT "Installing HDF5 library and header files ..."
    )
  add_dependencies (run_install run_make)

  ##__________________________________________________________________
  ## Post-installation clean-up: since the "--includedir" option does
  ## not seem to be handled properly by the configure script, we need
  ## to manually move the header files after installation

  add_custom_target (install ALL
    COMMAND mkdir -p hdf5 
    COMMAND mv H5*.h hdf5
    COMMAND mv hdf5*.h hdf5
    COMMAND rm -rf ${HDF5_BINARY_DIR}/hdf5-${HDF5_VERSION}
    WORKING_DIRECTORY ${HDF5_prefix}/include
    )
  add_dependencies (install run_install)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  message (STATUS "Found system-wide installation of HDF5; skipping rebuild!")
endif (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
