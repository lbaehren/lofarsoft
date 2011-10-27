
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (VALGRIND)

##__________________________________________________________
##  Package information

set (VALGRIND_VERSION        "3.6.1" )
set (VALGRIND_SOURCE_ARCHIVE "valgrind-${VALGRIND_VERSION}.tar.bz2" )
set (VALGRIND_URL            "http://valgrind.org/downloads/${VALGRIND_SOURCE_ARCHIVE}" )
set (VALGRIND_SVN            "svn://svn.valgrind.org/valgrind/trunk" )

##__________________________________________________________
## Package configuration/build options

option (VALGRIND_FORCE_BUILD  "Force build and installation of package?" NO )
option (VALGRIND_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

#find_package (Valgrind)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (VALGRIND_FROM_REPOS)
  set (VALGRIND_FORCE_BUILD TRUE)
endif (VALGRIND_FROM_REPOS)

if (NOT VALGRIND_FOUND OR VALGRIND_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (VALGRIND_SOURCE ${VALGRIND_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT VALGRIND_SOURCE AND VALGRIND_URL)
    set (VALGRIND_SOURCE ${VALGRIND_URL})
  endif (NOT VALGRIND_SOURCE AND VALGRIND_URL)
    
  if (VALGRIND_FROM_REPOS AND VALGRIND_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (valgrind
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${VALGRIND_SVN}
      BUILD_IN_SOURCE 1
      PATCH_COMMAND ${PROJECT_BINARY_DIR}/autogen.sh
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (VALGRIND_FROM_REPOS AND VALGRIND_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (valgrind
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${VALGRIND_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (VALGRIND_FROM_REPOS AND VALGRIND_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (valgrind update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of Valgrind ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT VALGRIND_FOUND OR VALGRIND_FORCE_BUILD)
  
  add_custom_target (valgrind
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[Valgrind] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT VALGRIND_FOUND OR VALGRIND_FORCE_BUILD)
