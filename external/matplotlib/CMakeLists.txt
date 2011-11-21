
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (MATPLOTLIB)

##__________________________________________________________
##  Package information

set (MATPLOTLIB_VERSION        "1.1.0" )
set (MATPLOTLIB_SOURCE_ARCHIVE "matplotlib-${MATPLOTLIB_VERSION}.tar.gz" )
set (MATPLOTLIB_URL            "http://downloads.sourceforge.net/project/matplotlib/matplotlib/matplotlib-${MATPLOTLIB_VERSION}/${MATPLOTLIB_SOURCE_ARCHIVE}" )
set (MATPLOTLIB_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (MATPLOTLIB_FORCE_BUILD  "Force build and installation of package?" NO )
option (MATPLOTLIB_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYTHON_FOUND)
  find_package (Python)
endif (NOT PYTHON_FOUND)

if (NOT NUMPY_FOUND)
  find_package (NumPy)
endif (NOT NUMPY_FOUND)

if (NOT MATPLOTLIB_FOUND)
  find_package (Matplotlib)
endif (NOT MATPLOTLIB_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (MATPLOTLIB_FROM_REPOS)
  set (MATPLOTLIB_FORCE_BUILD TRUE)
endif (MATPLOTLIB_FROM_REPOS)

if (NOT MATPLOTLIB_FOUND OR MATPLOTLIB_FORCE_BUILD)
  
  ##________________________________________________________
  ## Customize setup.py script

  configure_file (
    ${PROJECT_SOURCE_DIR}/setupext.py.in
    ${PROJECT_BINARY_DIR}/setupext.py
    )

  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (MATPLOTLIB_SOURCE ${MATPLOTLIB_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT MATPLOTLIB_SOURCE AND MATPLOTLIB_URL)
    set (MATPLOTLIB_SOURCE ${MATPLOTLIB_URL})
  endif (NOT MATPLOTLIB_SOURCE AND MATPLOTLIB_URL)
    
  if (MATPLOTLIB_FROM_REPOS AND MATPLOTLIB_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (matplotlib
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${MATPLOTLIB_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (MATPLOTLIB_FROM_REPOS AND MATPLOTLIB_SVN)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (matplotlib
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${MATPLOTLIB_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py install --prefix=  --home=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (MATPLOTLIB_FROM_REPOS AND MATPLOTLIB_SVN)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (matplotlib update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of matplotlib ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT MATPLOTLIB_FOUND OR MATPLOTLIB_FORCE_BUILD)
  
  add_custom_target (matplotlib
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[matplotlib] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT MATPLOTLIB_FOUND OR MATPLOTLIB_FORCE_BUILD)
