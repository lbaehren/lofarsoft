
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (FLEX)

##__________________________________________________________
##  Package information

set (FLEX_VERSION        "2.5.35" )
set (FLEX_SOURCE_ARCHIVE "flex-${FLEX_VERSION}.tar.gz" )
set (FLEX_URL            "http://downloads.sourceforge.net/project/flex/flex/flex-${FLEX_VERSION}/${FLEX_SOURCE_ARCHIVE}" )
set (FLEX_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (FLEX_FORCE_BUILD  "Force build and installation of package?" NO )
option (FLEX_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT FLEX_FOUND)
  find_package (FLEX)
endif (NOT FLEX_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (FLEX_FROM_REPOS)
  set (FLEX_FORCE_BUILD TRUE)
endif (FLEX_FROM_REPOS)

if (NOT FLEX_FOUND OR FLEX_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (FLEX_SOURCE ${FLEX_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT FLEX_SOURCE AND FLEX_URL)
    set (FLEX_SOURCE ${FLEX_URL})
  endif (NOT FLEX_SOURCE AND FLEX_URL)
    
  if (FLEX_FROM_REPOS AND FLEX_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (flex
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${FLEX_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (FLEX_FROM_REPOS AND FLEX_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (flex
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${FLEX_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (FLEX_FROM_REPOS AND FLEX_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (flex update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of flex ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT FLEX_FOUND OR FLEX_FORCE_BUILD)
  
  add_custom_target (flex
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[flex] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT FLEX_FOUND OR FLEX_FORCE_BUILD)
