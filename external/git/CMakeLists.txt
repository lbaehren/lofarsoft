
## Name of the project
project (GIT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Load CMake modules
include (ExternalProject)
  
## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (GIT_FORCE_BUILD       "Force rebuild and local installation?"  NO )
option (GIT_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO )
option (GIT_VERBOSE_BUILD     "Be verbose during build process?"       NO )

##_______________________________________________________________________________
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${GIT_SOURCE_DIR}
  ${GIT_SOURCE_DIR}/..
  ${GIT_SOURCE_DIR}/../..
  ${GIT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[git] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

##_______________________________________________________________________________
##                                                                   Dependencies

find_program (HAVE_GIT git ${bin_locations})

##_______________________________________________________________________________
##                                                                   Dependencies

find_program (asciidoc_executable asciidoc ${bin_locations})

## Curl : Though there typically is a system-wide installation of the library,
##        that one might be too old. In order to avoid tempering with the files
##        in /usr/lib or /usr/local/lib as newer custom version should be used
##        when building Git.

find_library (CURL_CURL_LIBRARY curl
  PATHS /sw /usr /usr/local /opt/local
  PATH_SUFFIXES lib
  )

find_path (CURL_INCLUDES include/curl/curl.h
  PATHS /sw /usr /usr/local /opt/local
  )

## SSL / OpenSSL

find_path (SSL_INCLUDES openssl/ssl2.h
  PATHS /sw /usr /usr/local /opt/local
  PATH_SUFFIXES include lib/openssl lib/openssl/include
  )

find_library (SSL_LIBRARIES ssl crypto
  PATHS /sw /usr /usr/local /opt/local
  PATH_SUFFIXES lib
  )

if (SSL_LIBRARIES)
  get_filename_component(SSL_LIBRARY_PATH ${SSL_LIBRARIES} PATH)
endif (SSL_LIBRARIES)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_GIT OR GIT_FORCE_BUILD)
  
  set (GIT_VERSION 1.7.1)
  set (GIT_SOURCE_ARCHIVE git-${GIT_VERSION}.tar.gz)
  set (GIT_URL http://kernel.org/pub/software/scm/git/${GIT_SOURCE_ARCHIVE})
  
  ## locate source file to build from
  find_file (HAVE_GIT_SOURCE ${GIT_SOURCE_ARCHIVE}
    PATHS ${GIT_SOURCE_DIR}
    )
  
  if (NOT HAVE_GIT_SOURCE)
    set (HAVE_GIT_SOURCE ${GIT_URL})
  endif (NOT HAVE_GIT_SOURCE)
  
  if (HAVE_GIT_SOURCE)
    ExternalProject_Add (git
      PREFIX ${GIT_BINARY_DIR}
      DOWNLOAD_DIR ${GIT_BINARY_DIR}
      SOURCE_DIR ${GIT_BINARY_DIR}
      URL ${HAVE_GIT_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GIT_BINARY_DIR}/configure prefix=${CMAKE_INSTALL_PREFIX} --with-curl=${CMAKE_INSTALL_PREFIX} --with-openssl=${SSL_LIBRARY_PATH}
      INSTALL_COMMAND make install
      )
  else (HAVE_GIT_SOURCE)
    message (WARNING "[git] Unable to build project - no sources found!")
  endif (HAVE_GIT_SOURCE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_GIT OR GIT_FORCE_BUILD)
  add_custom_target (git
    WORKING_DIRECTORY ${GIT_BINARY_DIR}
    COMMENT "[git] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_GIT OR GIT_FORCE_BUILD)
