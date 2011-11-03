
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (SPHINX)

##__________________________________________________________
##  Package information

set (SPHINX_VERSION        "1.1" )
set (SPHINX_SOURCE_ARCHIVE "Sphinx-${SPHINX_VERSION}.tar.gz" )
set (SPHINX_URL            "http://pypi.python.org/packages/source/S/Sphinx/${SPHINX_SOURCE_ARCHIVE}" )
set (SPHINX_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (SPHINX_FORCE_BUILD  "Force build and installation of package?" NO )
option (SPHINX_FROM_REPOS   "Install package from repository?"         NO )
option (SPHINX_REQUIRED_MAJOR_VERSION "Minimum required major version" 1  )
option (SPHINX_REQUIRED_MINOR_VERSION "Minimum required minor version" 0  )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYTHON_FOUND)
  find_package (Python)
endif (NOT PYTHON_FOUND)

##__________________________________________________________
## Existing installation of Sphinx?

if (NOT SPHINX_FOUND)
  find_package (Sphinx)
endif (NOT SPHINX_FOUND)

if (SPHINX_API_VERSION)
  if (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
    message (STATUS "[Sphinx] Installation found - Major version too small!")
    set (SPHINX_FORCE_BUILD TRUE)
  else (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (SPHINX_MINOR_VERSION LESS ${SPHINX_REQUIRED_MINOR_VERSION})
      message (STATUS "[Sphinx] Installation found - minor version too small!")
      set (SPHINX_FORCE_BUILD TRUE)
    endif (SPHINX_MINOR_VERSION LESS ${SPHINX_REQUIRED_MINOR_VERSION})
  endif (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
endif (SPHINX_API_VERSION)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (SPHINX_FROM_REPOS)
  set (SPHINX_FORCE_BUILD TRUE)
endif (SPHINX_FROM_REPOS)

if (NOT SPHINX_FOUND OR SPHINX_FORCE_BUILD)
  
  ##________________________________________________________
  ## Generate site.cfg file to custimize configuration

  configure_file (
    ${PROJECT_SOURCE_DIR}/site.cfg.in
    ${PROJECT_BINARY_DIR}/site.cfg
    )
  
  ##________________________________________________________
  ## Check if installation directory exists
  
  if (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/lib/python)
    file (MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/python)
  endif ()

  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (SPHINX_SOURCE ${SPHINX_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT SPHINX_SOURCE AND SPHINX_URL)
    set (SPHINX_SOURCE ${SPHINX_URL})
  endif (NOT SPHINX_SOURCE AND SPHINX_URL)
    
  if (SPHINX_FROM_REPOS AND SPHINX_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (sphinx
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${SPHINX_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND cat ${PROJECT_BINARY_DIR}/site.cfg >> setup.cfg
      BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
      INSTALL_COMMAND  ${CMAKE_COMMAND} -E environment "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib/python"&& ${PYTHON_EXECUTABLE} setup.py install --home=${CMAKE_INSTALL_PREFIX}
      )
  
  else (SPHINX_FROM_REPOS AND SPHINX_SVN)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (sphinx
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${SPHINX_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND cat ${PROJECT_BINARY_DIR}/site.cfg >> setup.cfg
      BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
      INSTALL_COMMAND  ${CMAKE_COMMAND} -E environment "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib/python"&& ${PYTHON_EXECUTABLE} setup.py install --home=${CMAKE_INSTALL_PREFIX}
      )
    
  endif (SPHINX_FROM_REPOS AND SPHINX_SVN)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (sphinx update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of Sphinx ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT SPHINX_FOUND OR SPHINX_FORCE_BUILD)
  
  add_custom_target (sphinx
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[Sphinx] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT SPHINX_FOUND OR SPHINX_FORCE_BUILD)
