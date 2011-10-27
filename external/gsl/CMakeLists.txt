
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (GSL)

##__________________________________________________________
##  Package information

set (GSL_VERSION        "1.15" )
set (GSL_SOURCE_ARCHIVE "gsl-${GSL_VERSION}.tar.gz" )
set (GSL_URL            "ftp://ftp.gnu.org/gnu/gsl/${GSL_SOURCE_ARCHIVE}" )
set (GSL_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (GSL_FORCE_BUILD  "Force build and installation of package?" NO )
option (GSL_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT GSL_FOUND)
  find_package (GSL)
endif (NOT GSL_FOUND)
  
## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (GSL_FROM_REPOS)
  set (GSL_FORCE_BUILD TRUE)
endif (GSL_FROM_REPOS)

if (NOT GSL_FOUND OR GSL_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (GSL_SOURCE ${GSL_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT GSL_SOURCE AND GSL_URL)
    set (GSL_SOURCE ${GSL_URL})
  endif (NOT GSL_SOURCE AND GSL_URL)
    
  if (GSL_FROM_REPOS AND GSL_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (gsl
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${GSL_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      INSTALL_COMMAND make install
      )
  
  else (GSL_FROM_REPOS AND GSL_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (gsl
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${GSL_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      INSTALL_COMMAND make install
      )
    
  endif (GSL_FROM_REPOS AND GSL_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (gsl update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of gsl ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT GSL_FOUND OR GSL_FORCE_BUILD)
  
  add_custom_target (gsl
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[gsl] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT GSL_FOUND OR GSL_FORCE_BUILD)
