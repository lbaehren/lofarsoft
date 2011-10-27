
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (ARMADILLO)

##__________________________________________________________
##  Package information

set (ARMADILLO_VERSION        "2.2.4" )
set (ARMADILLO_SOURCE_ARCHIVE "armadillo-${ARMADILLO_VERSION}.tar.gz" )
set (ARMADILLO_URL            "http://downloads.sourceforge.net/project/arma/${ARMADILLO_SOURCE_ARCHIVE}" )
set (ARMADILLO_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (ARMADILLO_FORCE_BUILD  "Force build and installation of package?" NO )
option (ARMADILLO_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

find_package (Armadillo)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (ARMADILLO_FROM_REPOS)
  set (ARMADILLO_FORCE_BUILD TRUE)
endif (ARMADILLO_FROM_REPOS)

if (NOT ARMADILLO_FOUND OR ARMADILLO_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (ARMADILLO_SOURCE ${ARMADILLO_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT ARMADILLO_SOURCE AND ARMADILLO_URL)
    set (ARMADILLO_SOURCE ${ARMADILLO_URL})
  endif (NOT ARMADILLO_SOURCE AND ARMADILLO_URL)
    
  if (ARMADILLO_FROM_REPOS AND ARMADILLO_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (armadillo
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${ARMADILLO_SVN}
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      )
  
  else (ARMADILLO_FROM_REPOS AND ARMADILLO_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (armadillo
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${ARMADILLO_SOURCE}
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (ARMADILLO_FROM_REPOS AND ARMADILLO_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (armadillo update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of armadillo ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT ARMADILLO_FOUND OR ARMADILLO_FORCE_BUILD)
  
  add_custom_target (armadillo
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[armadillo] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT ARMADILLO_FOUND OR ARMADILLO_FORCE_BUILD)
