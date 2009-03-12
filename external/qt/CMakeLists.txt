
## Name of the project
project (QT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (QT_FORCE_BUILD       "Force rebuild and local installation?"   NO  )
option (QT_CMAKE_BUILD       "Use CMake to configure and build QT?"    YES )
option (QT_VERBOSE_CONFIGURE "Be verbose during configuration?"        NO  )
option (QT_VERBOSE_BUILD     "Be verbose during build process?"        NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${QT_SOURCE_DIR}
  ${QT_SOURCE_DIR}/..
  ${QT_SOURCE_DIR}/../..
  ${QT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindQt4)

if (QT4_FOUND)
  ## set internal variable
  set (HAVE_QT YES)
  ## Summary about the version of Qt found on the system
  message (STATUS "Have Qt                       = ${QT_FOUND}")
  message (STATUS "Have Qt4                      = ${QT4_FOUND}")
  message (STATUS "The major version of Qt found = ${QT_VERSION_MAJOR}")
  message (STATUS "The minor version of Qt found = ${QT_VERSION_MINOR}")
  message (STATUS "The patch version of Qt found = ${QT_VERSION_PATCH}")
  message (STATUS "The edition of Qt             = ${QT_EDITION}")
endif (QT4_FOUND)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_QT OR QT_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (QT_prefix release_area.txt
    PATHS
    ${QT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT QT_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT QT_prefix)
  
  ## ------------------------------------------------------------------------------
  ## Check for required tools

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  ##__________________________________________________________________
  ## Download the tar-archive from the USG server
  
  if (UNIX)
    if (APPLE)
      set (QT_SYSTEM "mac")
    else (APPLE)
      set (QT_SYSTEM "x11")
    endif (APPLE)
  endif (UNIX)
  set (QT_BUILD_DIR "qt-${QT_SYSTEM}-opensource-src-4.4.0")
  set (QT_TAR_ARCHIVE "${QT_BUILD_DIR}.tar.gz")
  
  set (QT_URL "${USG_DOWNLOAD}/software/${QT_TAR_ARCHIVE}")

  if (QT_VERBOSE_CONFIGURE)
    message (STATUS "Qt download URL   = ${QT_URL}")
    message (STATUS "wget binary       = ${HAVE_WGET}")
    message (STATUS "curl binary       = ${HAVE_CURL}")
  endif (QT_VERBOSE_CONFIGURE)
  
  if (HAVE_WGET)
    add_custom_target (download_archive ALL
      COMMAND wget -c ${QT_URL}
      WORKING_DIRECTORY ${QT_BINARY_DIR}
      )
  else (HAVE_WGET)
    if (HAVE_CURL)
      add_custom_target (download_archive ALL
	COMMAND curl ${QT_URL} -o ${QT_TAR_ARCHIVE} -C -
	WORKING_DIRECTORY ${QT_BINARY_DIR}
	)
    else (HAVE_CURL)
      message (FATAL_ERROR "[QT] No tool found to download tar-archive!")
    endif (HAVE_CURL)
  endif (HAVE_WGET)
  
  ##__________________________________________________________________
  ## Expand the downloaded tar-archive
  
  add_custom_target (expand_archive ALL
    COMMAND tar -xvzf ${QT_TAR_ARCHIVE}
    WORKING_DIRECTORY ${QT_BINARY_DIR}
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure
  
  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${QT_prefix}
    WORKING_DIRECTORY ${QT_BINARY_DIR}/${QT_BUILD_DIR}
    )
  add_dependencies (run_configure expand_archive)
  
  ##__________________________________________________________________
  ## Build
  
  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${QT_BINARY_DIR}/${QT_BUILD_DIR}
    )
  add_dependencies (run_make run_configure)

  ##__________________________________________________________________
  ## Installation

  add_custom_target (install ALL
    COMMAND make install
    WORKING_DIRECTORY ${QT_BINARY_DIR}/${QT_BUILD_DIR}
    )
  add_dependencies (install run_make)

  ##__________________________________________________________________
  ## Installation

  add_custom_target (cleanup_build ALL
    COMMAND rm -rf qt-*
    WORKING_DIRECTORY ${QT_BINARY_DIR}
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_QT OR QT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of QT; skipping rebuild!")
endif (NOT HAVE_QT OR QT_FORCE_BUILD)
