
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (NUMPY)

##__________________________________________________________
##  Package information

set (NUMPY_REQUIRED_MAJOR_VERSION   1)
set (NUMPY_REQUIRED_MINOR_VERSION   1)

set (NUMPY_VERSION        "1.6.1" )
set (NUMPY_SOURCE_ARCHIVE "numpy-${NUMPY_VERSION}.tar.gz" )
set (NUMPY_URL            "http://downloads.sourceforge.net/project/numpy/NumPy/${NUMPY_VERSION}/${NUMPY_SOURCE_ARCHIVE}" )
set (NUMPY_GIT            "git://github.com/numpy/numpy.git" )

##__________________________________________________________
## Package configuration/build options

option (NUMPY_FORCE_BUILD  "Force build and installation of package?" NO )
option (NUMPY_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT NUMPY_FOUND)
  find_package (NumPy)
endif (NOT NUMPY_FOUND)

##__________________________________________________________
## Check version of NumPy

if (NUMPY_API_VERSION)
  if (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
    message (STATUS "[NumPy] Installation found - Major version too small!")
    set (NUMPY_FORCE_BUILD TRUE)
  else (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (NUMPY_MINOR_VERSION LESS ${NUMPY_REQUIRED_MINOR_VERSION})
      message (STATUS "[NumPy] Installation found - minor version too small!")
      set (NUMPY_FORCE_BUILD TRUE)
    endif (NUMPY_MINOR_VERSION LESS ${NUMPY_REQUIRED_MINOR_VERSION})
  endif (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
endif (NUMPY_API_VERSION)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (NUMPY_FROM_REPOS)
  set (NUMPY_FORCE_BUILD TRUE)
endif (NUMPY_FROM_REPOS)

if (NOT NUMPY_FOUND OR NUMPY_FORCE_BUILD)
  
  ##________________________________________________________
  ## Create "site.cfg" used later on

  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  configure_file (
    ${PROJECT_SOURCE_DIR}/site.cfg.in
    ${PROJECT_BINARY_DIR}/site.cfg
    )

  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (NUMPY_SOURCE ${NUMPY_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT NUMPY_SOURCE AND NUMPY_URL)
    set (NUMPY_SOURCE ${NUMPY_URL})
  endif (NOT NUMPY_SOURCE AND NUMPY_URL)
    
  if (NUMPY_FROM_REPOS AND NUMPY_GIT)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (numpy
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      GIT_REPOSITORY ${NUMPY_GIT}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND cp ${PROJECT_BINARY_DIR}/site.cfg ${PROJECT_BINARY_DIR}/source
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py install --prefix= --home=${CMAKE_INSTALL_PREFIX}
      )
  
  else (NUMPY_FROM_REPOS AND NUMPY_GIT)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (numpy
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${NUMPY_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND cp ${PROJECT_BINARY_DIR}/site.cfg ${NUMPY_BINARY_DIR}/source
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/setup.py install --prefix= --home=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (NUMPY_FROM_REPOS AND NUMPY_GIT)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (numpy update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of numpy ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT NUMPY_FOUND OR NUMPY_FORCE_BUILD)
  
  add_custom_target (numpy
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[numpy] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT NUMPY_FOUND OR NUMPY_FORCE_BUILD)
