
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (PLPLOT)

##__________________________________________________________
##  Package information

set (PLPLOT_VERSION        "5.9.4" )
set (PLPLOT_SOURCE_ARCHIVE "plplot-${PLPLOT_VERSION}.tar.gz" )
set (PLPLOT_URL            "http://downloads.sourceforge.net/project/plplot/plplot/${PLPLOT_VERSION}%20Source/${PLPLOT_SOURCE_ARCHIVE}" )
set (PLPLOT_SVN            "https://plplot.svn.sourceforge.net/svnroot/plplot/trunk" )

##__________________________________________________________
## Package configuration/build options

option (PLPLOT_FORCE_BUILD  "Force build and installation of package?" NO )
option (PLPLOT_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PLPLOT_FOUND)
  find_package (PLplot)
endif (NOT PLPLOT_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (PLPLOT_FROM_REPOS)
  set (PLPLOT_FORCE_BUILD TRUE)
endif (PLPLOT_FROM_REPOS)

if (NOT PLPLOT_FOUND OR PLPLOT_FORCE_BUILD)

  ##________________________________________________________
  ## Initial CMake cache variables
  
  configure_file (
    ${PLPLOT_SOURCE_DIR}/plplot.cmake.in
    ${PLPLOT_BINARY_DIR}/plplot.cmake
    )

  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (PLPLOT_SOURCE ${PLPLOT_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT PLPLOT_SOURCE AND PLPLOT_URL)
    set (PLPLOT_SOURCE ${PLPLOT_URL})
  endif (NOT PLPLOT_SOURCE AND PLPLOT_URL)
    
  if (PLPLOT_FROM_REPOS AND PLPLOT_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (plplot
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${PLPLOT_SVN}
      CMAKE_ARGS -C ${PROJECT_BINARY_DIR}/plplot.cmake
      )
  
  else (PLPLOT_FROM_REPOS AND PLPLOT_SVN)
    
    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (plplot
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${PLPLOT_SOURCE}
      CMAKE_ARGS -C ${PROJECT_BINARY_DIR}/plplot.cmake
      )
    
  endif (PLPLOT_FROM_REPOS AND PLPLOT_SVN)
  
  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (plplot update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of PLplot ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT PLPLOT_FOUND OR PLPLOT_FORCE_BUILD)
  
  add_custom_target (plplot
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[PLplot] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT PLPLOT_FOUND OR PLPLOT_FORCE_BUILD)
