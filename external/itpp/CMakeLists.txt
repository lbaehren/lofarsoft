
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (ITPP)

##__________________________________________________________
##  Package information

set (ITPP_VERSION_MAJOR  "4" )
set (ITPP_VERSION_MINOR  "2" )
set (ITPP_VERSION_PATCH  "0" )
set (ITPP_VERSION        "${ITPP_VERSION_MAJOR}.${ITPP_VERSION_MINOR}.${ITPP_VERSION_PATCH}" )
set (ITPP_SOURCE_ARCHIVE "itpp-${ITPP_VERSION_MAJOR}.${ITPP_VERSION_MINOR}.tar.gz" )
set (ITPP_URL            "http://downloads.sourceforge.net/project/itpp/itpp/${ITPP_VERSION}/${ITPP_SOURCE_ARCHIVE}" )
set (ITPP_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (ITPP_FORCE_BUILD  "Force build and installation of package?" NO )
option (ITPP_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT ITPP_FOUND)
  find_package (ITPP)
endif (NOT ITPP_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (ITPP_FROM_REPOS)
  set (ITPP_FORCE_BUILD TRUE)
endif (ITPP_FROM_REPOS)

if (NOT ITPP_FOUND OR ITPP_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (ITPP_SOURCE ${ITPP_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT ITPP_SOURCE AND ITPP_URL)
    set (ITPP_SOURCE ${ITPP_URL})
  endif (NOT ITPP_SOURCE AND ITPP_URL)
    
  if (ITPP_FROM_REPOS AND ITPP_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (itpp
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${ITPP_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --without-blas --without-lapack --without-fft --disable-html-doc
      )
  
  else (ITPP_FROM_REPOS AND ITPP_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (itpp
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${ITPP_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --without-blas --without-lapack --without-fft --disable-html-doc
      )
    
  endif (ITPP_FROM_REPOS AND ITPP_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (itpp update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of IT++ ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT ITPP_FOUND OR ITPP_FORCE_BUILD)
  
  add_custom_target (itpp
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[IT++] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT ITPP_FOUND OR ITPP_FORCE_BUILD)
