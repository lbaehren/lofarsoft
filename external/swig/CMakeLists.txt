
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (SWIG)

##__________________________________________________________
##  Package information

set (SWIG_VERSION        "2.0.4" )
set (SWIG_SOURCE_ARCHIVE "swig-${SWIG_VERSION}.tar.gz" )
set (SWIG_URL            "http://downloads.sourceforge.net/project/swig/swig/swig-${SWIG_VERSION}/${SWIG_SOURCE_ARCHIVE}" )
set (SWIG_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (SWIG_FORCE_BUILD  "Force build and installation of package?" NO )
option (SWIG_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT SWIG_FOUND)
  find_package (SWIG)
endif (NOT SWIG_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (SWIG_FROM_REPOS)
  set (SWIG_FORCE_BUILD TRUE)
endif (SWIG_FROM_REPOS)

if (NOT SWIG_FOUND OR SWIG_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (SWIG_SOURCE ${SWIG_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT SWIG_SOURCE AND SWIG_URL)
    set (SWIG_SOURCE ${SWIG_URL})
  endif (NOT SWIG_SOURCE AND SWIG_URL)
    
  if (SWIG_FROM_REPOS AND SWIG_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (swig
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${SWIG_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (SWIG_FROM_REPOS AND SWIG_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (swig
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${SWIG_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (SWIG_FROM_REPOS AND SWIG_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (swig update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of swig ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT SWIG_FOUND OR SWIG_FORCE_BUILD)
  
  add_custom_target (swig
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[swig] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT SWIG_FOUND OR SWIG_FORCE_BUILD)
