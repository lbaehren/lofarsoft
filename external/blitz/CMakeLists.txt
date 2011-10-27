
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (BLITZ)

##__________________________________________________________
##  Package information

set (BLITZ_VERSION        "0.9" )
set (BLITZ_SOURCE_ARCHIVE "blitz-${BLITZ_VERSION}.tar.gz" )
set (BLITZ_URL            "http://downloads.sourceforge.net/project/blitz/blitz/Blitz%2B%2B%20${BLITZ_VERSION}/${BLITZ_SOURCE_ARCHIVE}" )
set (BLITZ_CVS            ":pserver:anonymous@blitz.cvs.sourceforge.net:/cvsroot/blitz" )

##__________________________________________________________
## Package configuration/build options

option (BLITZ_FORCE_BUILD  "Force build and installation of package?" NO )
option (BLITZ_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

find_package (Blitz)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (BLITZ_FROM_REPOS)
  set (BLITZ_FORCE_BUILD TRUE)
endif (BLITZ_FROM_REPOS)

if (NOT BLITZ_FOUND OR BLITZ_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (BLITZ_SOURCE ${BLITZ_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT BLITZ_SOURCE AND BLITZ_URL)
    set (BLITZ_SOURCE ${BLITZ_URL})
  endif (NOT BLITZ_SOURCE AND BLITZ_URL)
    
  if (BLITZ_FROM_REPOS AND BLITZ_CVS)

    ## Build package off code repository
    
    ExternalProject_Add (blitz
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      CVS_REPOSITORY ${BLITZ_CVS}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --disable-doxygen
      )
  
  else (BLITZ_FROM_REPOS AND BLITZ_CVS)

    ## Build package off tar-archive
    
    ExternalProject_Add (blitz
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${BLITZ_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --disable-doxygen
      )
    
  endif (BLITZ_FROM_REPOS AND BLITZ_CVS)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (blitz update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of blitz ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT BLITZ_FOUND OR BLITZ_FORCE_BUILD)
  
  add_custom_target (blitz
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[blitz] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT BLITZ_FOUND OR BLITZ_FORCE_BUILD)
