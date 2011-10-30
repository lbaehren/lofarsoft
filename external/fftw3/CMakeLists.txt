
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (FFTW3)

##__________________________________________________________
##  Package information

set (FFTW3_VERSION        "3.3" )
set (FFTW3_SOURCE_ARCHIVE "fftw-${FFTW3_VERSION}.tar.gz" )
set (FFTW3_URL            "ftp://ftp.fftw.org/pub/fftw/${FFTW3_SOURCE_ARCHIVE}" )
set (FFTW3_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (FFTW3_FORCE_BUILD  "Force build and installation of package?" NO )
option (FFTW3_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT FFTW3_FOUND)
  find_package (FFTW3)
endif (NOT FFTW3_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (FFTW3_FROM_REPOS)
  set (FFTW3_FORCE_BUILD TRUE)
endif (FFTW3_FROM_REPOS)

if (NOT FFTW3_FOUND OR FFTW3_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (FFTW3_SOURCE ${FFTW3_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT FFTW3_SOURCE AND FFTW3_URL)
    set (FFTW3_SOURCE ${FFTW3_URL})
  endif (NOT FFTW3_SOURCE AND FFTW3_URL)
    
  if (FFTW3_FROM_REPOS AND FFTW3_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (fftw3
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${FFTW3_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (FFTW3_FROM_REPOS AND FFTW3_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (fftw3
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${FFTW3_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (FFTW3_FROM_REPOS AND FFTW3_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (fftw3 update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of fftw3 ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT FFTW3_FOUND OR FFTW3_FORCE_BUILD)
  
  add_custom_target (fftw3
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[fftw3] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT FFTW3_FOUND OR FFTW3_FORCE_BUILD)
