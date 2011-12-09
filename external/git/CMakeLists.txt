
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
##                                                                   Dependencies

find_program (GIT_EXECUTABLE git)

## Curl : Though there typically is a system-wide installation of the library,
##        that one might be too old. In order to avoid tempering with the files
##        in /usr/lib or /usr/local/lib as newer custom version should be used
##        when building Git.

find_path    (CURL_INCLUDES     include/curl/curl.h)
find_library (CURL_CURL_LIBRARY curl)

## SSL / OpenSSL

find_path (SSL_INCLUDES openssl/ssl2.h
  PATH_SUFFIXES include lib/openssl lib/openssl/include
  )

find_library (SSL_LIBRARIES ssl crypto)

if (SSL_LIBRARIES)
  get_filename_component(SSL_LIBRARY_PATH ${SSL_LIBRARIES} PATH)
endif (SSL_LIBRARIES)

## <-- begin build condition --------------------------------------------------->

if (NOT GIT_EXECUTABLE OR GIT_FORCE_BUILD)
  
  set (GIT_VERSION 1.7.6)
  set (GIT_SOURCE_ARCHIVE git-${GIT_VERSION}.tar.gz)
  # kernel.org temporarily broken
  #set (GIT_URL http://kernel.org/pub/software/scm/git/${GIT_SOURCE_ARCHIVE})

  set (GIT_URL http://git-core.googlecode.com/files/git-1.7.6.4.tar.gz)

  ## locate source file to build from
  find_file (GIT_SOURCE ${GIT_SOURCE_ARCHIVE}
    PATHS ${GIT_SOURCE_DIR}
    )
  
  if (NOT GIT_SOURCE)
    set (GIT_SOURCE ${GIT_URL})
  endif (NOT GIT_SOURCE)
  
  if (GIT_SOURCE)
    ExternalProject_Add (git
      PREFIX ${GIT_BINARY_DIR}
      DOWNLOAD_DIR ${GIT_BINARY_DIR}/download
      SOURCE_DIR ${GIT_BINARY_DIR}/source
      URL ${GIT_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GIT_BINARY_DIR}/source/configure prefix=${CMAKE_INSTALL_PREFIX} --with-curl=${CMAKE_INSTALL_PREFIX} --with-openssl=${SSL_LIBRARY_PATH}
      INSTALL_COMMAND make install
      )
  else (GIT_SOURCE)
    message (WARNING "[git] Unable to build project - no sources found!")
  endif (GIT_SOURCE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT GIT_EXECUTABLE OR GIT_FORCE_BUILD)
  add_custom_target (git
    WORKING_DIRECTORY ${GIT_BINARY_DIR}
    COMMENT "[git] Found system-wide installation; skipping rebuild!"
    )
endif (NOT GIT_EXECUTABLE OR GIT_FORCE_BUILD)
