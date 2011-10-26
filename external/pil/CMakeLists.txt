
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (PIL)

##__________________________________________________________
##  Package information

set (PIL_VERSION        "" )
set (PIL_SOURCE_ARCHIVE "pil.tgz" )
set (PIL_URL            "" )
set (PIL_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (PIL_FORCE_BUILD  "Force build and installation of package?" NO )
option (PIL_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

include (FindPIL)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (PIL_FROM_REPOS)
  set (PIL_FORCE_BUILD TRUE)
endif (PIL_FROM_REPOS)

if (NOT PIL_FOUND OR PIL_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (PIL_SOURCE ${PIL_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT PIL_SOURCE AND PIL_URL)
    set (PIL_SOURCE ${PIL_URL})
  endif (NOT PIL_SOURCE AND PIL_URL)
    
  if (PIL_FROM_REPOS AND PIL_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (pil
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${PIL_SVN}
      BUILD_IN_SOURCE 1
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      )
  
  else (PIL_FROM_REPOS AND PIL_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (pil
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${PIL_SOURCE}
      BUILD_IN_SOURCE 1
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (PIL_FROM_REPOS AND PIL_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (pil update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of pil ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT PIL_FOUND OR PIL_FORCE_BUILD)
  
  add_custom_target (pil
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[pil] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT PIL_FOUND OR PIL_FORCE_BUILD)
