
## Name of the project
project (SIP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SIP_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (SIP_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (SIP_VERBOSE_BUILD     "Be verbose during build process?"       NO  )
option (SIP_CMAKE_BUILD       "Use CMake to configure and build?"      NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SIP_SOURCE_DIR}
  ${SIP_SOURCE_DIR}/..
  ${SIP_SOURCE_DIR}/../..
  ${SIP_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[SIP] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindSIP)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SIP OR SIP_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (SIP_prefix release_area.txt
    PATHS
    ${SIP_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT SIP_prefix)
    message (FATAL_ERROR "[SIP] Unable to locate the installation area")
  endif (NOT SIP_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (SIP_VERSION "4.8.1")
  set (SIP_DIRECTORY "sip-${SIP_VERSION}")
  set (SIP_TAR_ARCHIVE "${SIP_DIRECTORY}.tar.gz")
  set (SIP_URL "http://www.riverbankcomputing.co.uk/static/Downloads/sip4/${SIP_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the server.
  
  find_file (have_sip_tgz ${SIP_TAR_ARCHIVE} ${SIP_SOURCE_DIR})
  
  if (have_sip_tgz)
    add_custom_target (download_archive
      COMMAND cp ${have_sip_tgz} .
      WORKING_DIRECTORY ${SIP_BINARY_DIR}
      COMMENT "Copying tar-archive from ${USG_ROOT}/external/sip ..."
      )
  else (have_sip_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive
	COMMAND wget -c ${SIP_URL}
	WORKING_DIRECTORY ${SIP_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${SIP_URL} -o ${SIP_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${SIP_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[SIP] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_sip_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive
    COMMAND tar -xzf ${SIP_TAR_ARCHIVE}
    WORKING_DIRECTORY ${SIP_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure and build the project

  ## run "./configure"

  add_custom_target (run_configure ALL
    COMMAND python configure.py -b ${SIP_prefix}/bin -d ${SIP_prefix}/lib/python -e ${SIP_prefix}/include/sip -v ${SIP_prefix}/share/sip
    WORKING_DIRECTORY ${SIP_BINARY_DIR}/${SIP_DIRECTORY}
    COMMENT "[SIP] Running configure ..."
    )
  add_dependencies (run_configure expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${SIP_BINARY_DIR}/${SIP_DIRECTORY}
    COMMENT "[SIP] Starting build ..."
    )
  add_dependencies (run_make run_configure)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${SIP_BINARY_DIR}/${SIP_TAR_ARCHIVE}
    COMMAND rm -rf ${SIP_BINARY_DIR}/${SIP_DIRECTORY}
    WORKING_DIRECTORY ${SIP_BINARY_DIR}/${SIP_DIRECTORY}
    COMMENT "[SIP] Installing software and cleaning up ..."
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SIP OR SIP_FORCE_BUILD)
  message (STATUS "Found system-wide installation of SIP; skipping rebuild!")
endif (NOT HAVE_SIP OR SIP_FORCE_BUILD)
