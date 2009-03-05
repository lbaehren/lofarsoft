
## Name of the project
project (MATHGL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATHGL_FORCE_BUILD "Force rebuild and local installation?"     NO  )
option (MATHGL_CMAKE_BUILD "Use CMake to configure and build MATHGL?"  YES )
option (MATHGL_VERBOSE_CONFIGURE "Be verbose during configuration?"    NO  )
option (MATHGL_VERBOSE_BUILD     "Be verbose during build process?"    NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${MATHGL_SOURCE_DIR}
  ${MATHGL_SOURCE_DIR}/..
  ${MATHGL_SOURCE_DIR}/../..
  ${MATHGL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindMathGL)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (MATHGL_prefix release_area.txt
    PATHS
    ${MATHGL_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT MATHGL_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT MATHGL_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (MATHGL_TAR_ARCHIVE "mathgl.tgz")
  set (MATHGL_URL "${USG_DOWNLOAD}/software/${MATHGL_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_mathgl_tgz ${MATHGL_TAR_ARCHIVE} ${MATHGL_SOURCE_DIR})
  
  if (have_mathgl_tgz)
    add_custom_target (download_archive ALL
      COMMAND cp ${have_mathgl_tgz} .
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
      COMMENT "Copying tar-archive from USG_ROOT/external/mathgl ..."
      )
  else (have_mathgl_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive ALL
	COMMAND wget -c ${MATHGL_URL}
	WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${MATHGL_URL} -o ${MATHGL_TAR_ARCHIVE} -C
	  WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[MATHGL] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_mathgl_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive ALL
    COMMAND tar -xvzf ${MATHGL_TAR_ARCHIVE}
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    )
  add_dependencies (expand_archive download_archive)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  message (STATUS "Found system-wide installation of MATHGL; skipping rebuild!")
endif (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
