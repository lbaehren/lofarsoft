
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (NUMERIC)

##__________________________________________________________
##  Package information

set (NUMERIC_VERSION        "24.2" )
set (NUMERIC_SOURCE_ARCHIVE "Numeric-${NUMERIC_VERSION}.tar.gz" )
set (NUMERIC_URL            "http://downloads.sourceforge.net/project/numpy/Old%20Numeric/${NUMERIC_VERSION}/${NUMERIC_SOURCE_ARCHIVE}" )
set (NUMERIC_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (NUMERIC_FORCE_BUILD  "Force build and installation of package?" NO )
option (NUMERIC_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYTHON_EXECUTABLE)
  find_package (Python)
endif (NOT PYTHON_EXECUTABLE)

if (NOT NUMERIC_FOUND)
  find_package (Numeric)
endif (NOT NUMERIC_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (NUMERIC_FROM_REPOS)
  set (NUMERIC_FORCE_BUILD TRUE)
endif (NUMERIC_FROM_REPOS)

if (NOT NUMERIC_FOUND OR NUMERIC_FORCE_BUILD)
  
  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (NUMERIC_SOURCE ${NUMERIC_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT NUMERIC_SOURCE AND NUMERIC_URL)
    set (NUMERIC_SOURCE ${NUMERIC_URL})
  endif (NOT NUMERIC_SOURCE AND NUMERIC_URL)
    
  if (NUMERIC_FROM_REPOS AND NUMERIC_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (numeric
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${NUMERIC_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py build
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py install --home=${CMAKE_INSTALL_PREFIX}
      )
  
  else (NUMERIC_FROM_REPOS AND NUMERIC_SVN)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (numeric
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${NUMERIC_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py build
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py install --home=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (NUMERIC_FROM_REPOS AND NUMERIC_SVN)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (numeric update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of Numeric ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT NUMERIC_FOUND OR NUMERIC_FORCE_BUILD)
  
  add_custom_target (numeric
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[Numeric] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT NUMERIC_FOUND OR NUMERIC_FORCE_BUILD)
