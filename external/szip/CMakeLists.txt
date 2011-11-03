
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (SZIP)

##__________________________________________________________
##  Package information

set (SZIP_VERSION        "2.1" )
set (SZIP_SOURCE_ARCHIVE "szip-${SZIP_VERSION}.tar.gz" )
set (SZIP_URL            "http://www.hdfgroup.org/ftp/lib-external/szip/${SZIP_VERSION}/src/${SZIP_SOURCE_ARCHIVE}" )
set (SZIP_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (SZIP_FORCE_BUILD  "Force build and installation of package?" NO )
option (SZIP_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT SZIP_FOUND)
  find_package (SZIP)
endif (NOT SZIP_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (SZIP_FROM_REPOS)
  set (SZIP_FORCE_BUILD TRUE)
endif (SZIP_FROM_REPOS)

if (NOT SZIP_FOUND OR SZIP_FORCE_BUILD)
  
  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (SZIP_SOURCE ${SZIP_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT SZIP_SOURCE AND SZIP_URL)
    set (SZIP_SOURCE ${SZIP_URL})
  endif (NOT SZIP_SOURCE AND SZIP_URL)
    
  if (SZIP_FROM_REPOS AND SZIP_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (szip
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${SZIP_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (SZIP_FROM_REPOS AND SZIP_SVN)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (szip
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${SZIP_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (SZIP_FROM_REPOS AND SZIP_SVN)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (szip update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of szip ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT SZIP_FOUND OR SZIP_FORCE_BUILD)
  
  add_custom_target (szip
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[szip] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT SZIP_FOUND OR SZIP_FORCE_BUILD)
