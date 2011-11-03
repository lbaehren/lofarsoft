
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (IPYTHON)

##__________________________________________________________
##  Package information

set (IPYTHON_VERSION        "0.11" )
set (IPYTHON_SOURCE_ARCHIVE "ipython-${IPYTHON_VERSION}.tar.gz" )
set (IPYTHON_URL            "http://archive.ipython.org/release/${IPYTHON_VERSION}/${IPYTHON_SOURCE_ARCHIVE}" )
set (IPYTHON_GIT            "https://github.com/ipython/ipython.git" )

##__________________________________________________________
## Package configuration/build options

option (IPYTHON_FORCE_BUILD  "Force build and installation of package?"   NO )
option (IPYTHON_FROM_REPOS   "Install package from repository?"           NO )
option (IPYTHON_REQUIRED_MAJOR_VERSION "Major version of IPython release" 0  )
option (IPYTHON_REQUIRED_MINOR_VERSION "Minor version of IPython release" 10 )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYTHON_FOUND)
  find_package (Python)
endif (NOT PYTHON_FOUND)

if (NOT IPYTHON_FOUND)
  find_package (IPython)
endif (NOT IPYTHON_FOUND)

if (IPYTHON_API_VERSION)
  if (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
    message (STATUS "[IPython] Installation found - Major version too small!")
    set (IPYTHON_FORCE_BUILD TRUE)
  else (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (IPYTHON_MINOR_VERSION LESS ${IPYTHON_REQUIRED_MINOR_VERSION})
      message (STATUS "[IPython] Installation found - minor version too small!")
      set (IPYTHON_FORCE_BUILD TRUE)
    endif (IPYTHON_MINOR_VERSION LESS ${IPYTHON_REQUIRED_MINOR_VERSION})
  endif (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
endif (IPYTHON_API_VERSION)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (IPYTHON_FROM_REPOS)
  set (IPYTHON_FORCE_BUILD TRUE)
endif (IPYTHON_FROM_REPOS)

if (NOT IPYTHON_FOUND OR IPYTHON_FORCE_BUILD)
  
  ##________________________________________________________
  ## Create "site.cfg" used later on
  
  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  file (WRITE ${PROJECT_BINARY_DIR}/site.cfg "[DEFAULT] \n")
  file (APPEND ${PROJECT_BINARY_DIR}/site.cfg "library_dirs = /usr/lib, /usr/local/lib, /usr/lib64, /usr/local/lib64, /sw/lib, ${python_libdir} \n")
  
  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (IPYTHON_SOURCE ${IPYTHON_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT IPYTHON_SOURCE AND IPYTHON_URL)
    set (IPYTHON_SOURCE ${IPYTHON_URL})
  endif (NOT IPYTHON_SOURCE AND IPYTHON_URL)
    
  if (IPYTHON_FROM_REPOS AND IPYTHON_GIT)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (ipython
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      GIT_REPOSITORY ${IPYTHON_GIT}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/site.cfg site.cfg
      BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${CMAKE_INSTALL_PREFIX}
      )
  
  else (IPYTHON_FROM_REPOS AND IPYTHON_GIT)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (ipython
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${IPYTHON_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/site.cfg site.cfg
      BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (IPYTHON_FROM_REPOS AND IPYTHON_GIT)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (ipython update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of IPython ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT IPYTHON_FOUND OR IPYTHON_FORCE_BUILD)
  
  add_custom_target (ipython
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[IPython] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT IPYTHON_FOUND OR IPYTHON_FORCE_BUILD)
