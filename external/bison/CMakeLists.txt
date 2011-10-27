
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (BISON)

##__________________________________________________________
##  Package information

set (BISON_VERSION        "2.5" )
set (BISON_SOURCE_ARCHIVE "bison-${BISON_VERSION}.tar.gz" )
set (BISON_URL            "ftp://ftp.gnu.org/gnu/bison/${BISON_SOURCE_ARCHIVE}" )
set (BISON_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (BISON_FORCE_BUILD  "Force build and installation of package?" NO )
option (BISON_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT BISON_FOUND)
  find_package (BISON)
endif (NOT BISON_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (BISON_FROM_REPOS)
  set (BISON_FORCE_BUILD TRUE)
endif (BISON_FROM_REPOS)

if (NOT BISON_FOUND OR BISON_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (BISON_SOURCE ${BISON_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT BISON_SOURCE AND BISON_URL)
    set (BISON_SOURCE ${BISON_URL})
  endif (NOT BISON_SOURCE AND BISON_URL)
    
  if (BISON_FROM_REPOS AND BISON_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (bison
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${BISON_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (BISON_FROM_REPOS AND BISON_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (bison
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${BISON_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (BISON_FROM_REPOS AND BISON_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (bison update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of bison ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT BISON_FOUND OR BISON_FORCE_BUILD)
  
  add_custom_target (bison
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[bison] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT BISON_FOUND OR BISON_FORCE_BUILD)
