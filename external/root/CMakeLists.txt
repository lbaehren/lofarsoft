
## Name of the project
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ROOT_FORCE_BUILD       "Force rebuild and local installation?"   NO  )
option (ROOT_CMAKE_BUILD       "Use CMake to configure and build ROOT?"  YES )
option (ROOT_VERBOSE_CONFIGURE "Be verbose during configuration?"        NO  )
option (ROOT_VERBOSE_BUILD     "Be verbose during build process?"        NO  )
option (ROOT_SVN_BUILD         "Build Root from current SVN version?"    NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ROOT_SOURCE_DIR}
  ${ROOT_SOURCE_DIR}/..
  ${ROOT_SOURCE_DIR}/../..
  ${ROOT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT ROOT_VERBOSE_CONFIGURE)
  set (ROOT_FIND_QUIETLY YES)
endif (NOT ROOT_VERBOSE_CONFIGURE)

include (FindROOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)

  include (ExternalProject)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (ROOT_prefix release_area.txt
    PATHS
    ${ROOT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT ROOT_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT ROOT_prefix)

  ##__________________________________________________________________
  ## Build Root from latest SVN version

  if (ROOT_SVN_BUILD) 
    
    if (UNIX)
      if (APPLE)
	set (root_architecture "macosx")
      else (APPLE)
	set (root_architecture "linux")
      endif (APPLE)
    else (UNIX)
      if (WIN32)
	set (root_architecture "win32gcc")
      endif (WIN32)
    endif (UNIX)
    
    ExternalProject_Add (Root
      PREFIX ${USG_ROOT}/build/root
      DOWNLOAD_DIR ${ROOT_BINARY_DIR}/Root
      SOURCE_DIR ${ROOT_BINARY_DIR}/Root
      SVN_REPOSITORY http://root.cern.ch/svn/root/trunk
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${ROOT_BINARY_DIR}/Root/configure ${root_architecture} --prefix=${ROOT_prefix}
      )
  endif (ROOT_SVN_BUILD)
  
  ##__________________________________________________________________
  ## Build Root from release version

  if (NOT ROOT_SVN_BUILD)
    
    ## Check for required tools
    
    find_program (HAVE_WGET  wget  ${bin_locations} )
    find_program (HAVE_CURL  curl  ${bin_locations} )
    find_program (HAVE_TAR   tar   ${bin_locations} )
    
    ## Locate required libraries
    
    list (APPEND lib_locations /usr/lib64)
    list (APPEND lib_locations /usr/local/lib64)
    list (APPEND lib_locations /usr/X11R6/lib)
    list (APPEND lib_locations /usr/X11R6/lib64)
    
    find_file (libXpm libXpm.so.0 libXpm.so.1 libXpm.so.2 libXpm.so.3 libXpm.so.4
      PATHS ${lib_locations}
      )
    
    ## create symbolic link to ensure the library is found
    if (libXpm)
      add_custom_target(symlink_libXpm
	COMMAND rm -f libXpm.so
	COMMAND ln -s ${libXpm} libXpm.so
	WORKING_DIRECTORY ${ROOT_prefix}/lib
	COMMENT "Creating symbolic link for libXpm ..."
	)
    endif (libXpm)
    
    ## Download the tar-archive from the USG server
    
    set (ROOT_VERSION "v5.22.00")
    set (ROOT_TAR_ARCHIVE "root_${ROOT_VERSION}.source.tar.gz")
    set (ROOT_URL "${USG_DOWNLOAD}/software/${ROOT_TAR_ARCHIVE}")
    
    if (HAVE_WGET)
      add_custom_target (download_archive ALL
	COMMAND wget -c ${ROOT_URL}
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${ROOT_URL} -o ${ROOT_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[ROOT] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
    
    ## Expand the downloaded tar-archive
    
    if (HAVE_TAR)
      add_custom_target (expand_archive ALL
	COMMAND tar -xvzf ${ROOT_TAR_ARCHIVE}
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	COMMENT "[ROOT] Expanding compressed tar-archive ..."
	)
      add_dependencies (expand_archive download_archive)
    else (HAVE_TAR)
      message (FATAL_ERROR "[ROOT] Missing tar to expand downloaded archive!")
    endif (HAVE_TAR)
    
    ## Configure, build and install
    
    if (libXpm)
      add_custom_target (run_configure ALL
	COMMAND ./configure --prefix=${ROOT_prefix} --with-xpm-libdir=${ROOT_prefix}/lib
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
	COMMENT "[ROOT] Running configure ..."
	)
      add_dependencies (run_configure expand_archive symlink_libXpm)
    else (libXpm)
      add_custom_target (run_configure ALL
	COMMAND ./configure --prefix=${ROOT_prefix}
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
	COMMENT "[ROOT] Running configure ..."
	)
      add_dependencies (run_configure expand_archive)
    endif (libXpm)
    
    add_custom_target (run_make ALL
      COMMAND make
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
      COMMENT "[ROOT] Running make ..."
      )
    add_dependencies (run_make run_configure)
    
    add_custom_target (install ALL
      COMMAND make install
      COMMAND rm -rf root
      COMMAND rm -rf ${ROOT_TAR_ARCHIVE}
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
      COMMENT "[ROOT] Running installation ..."
      )
    add_dependencies (install run_make)
    
    ## Clean up the build directory
    
    add_custom_target (cleanup_build ALL
      COMMAND rm -f ${ROOT_TAR_ARCHIVE}
      COMMAND rm -rf root
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}
      )

  endif (NOT ROOT_SVN_BUILD)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of ROOT; skipping rebuild!")
endif (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
