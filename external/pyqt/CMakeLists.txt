
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (PYQT)

##__________________________________________________________
##  Package information

set (PYQT_VERSION "4.8.6" )

if (UNIX)
  if (APPLE)
    set (PYQT_SOURCE_ARCHIVE "PyQt-mac-gpl-${PYQT_VERSION}.tar.gz" )
  else (APPLE)
    set (PYQT_SOURCE_ARCHIVE "PyQt-x11-gpl-${PYQT_VERSION}.tar.gz" )
  endif (APPLE)
else (UNIX)
    set (PYQT_SOURCE_ARCHIVE "PyQt-win-gpl-${PYQT_VERSION}.zip" )
endif (UNIX)

set (PYQT_URL  "http://www.riverbankcomputing.co.uk/static/Downloads/PyQt4/${PYQT_SOURCE_ARCHIVE}" )
set (PYQT_SVN  "UNDEFINED" )

##__________________________________________________________
## Package configuration/build options

option (PYQT_FORCE_BUILD  "Force build and installation of package?" NO )
option (PYQT_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYQT_FOUND)
  find_package (PyQt)
endif (NOT PYQT_FOUND)

if (NOT QT_FOUND)
  find_package (Qt4)
endif (NOT QT_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (PYQT_FROM_REPOS)
  set (PYQT_FORCE_BUILD TRUE)
endif (PYQT_FROM_REPOS)

if (NOT PYQT_FOUND OR PYQT_FORCE_BUILD)
 
  ##________________________________________________________
  ## Build conditions
  
  if (PYTHON_EXECUTABLE)

    include (ExternalProject)
    
    ##________________________________________________________
    ## Locate source file to build from
    
    find_file (PYQT_SOURCE ${PYQT_SOURCE_ARCHIVE}
      PATHS ${PROJECT_SOURCE_DIR}
      )
    
    if (NOT PYQT_SOURCE AND PYQT_URL)
      set (PYQT_SOURCE ${PYQT_URL})
    endif (NOT PYQT_SOURCE AND PYQT_URL)
    
    if (PYQT_FROM_REPOS AND PYQT_SVN)
      
      ##______________________________________________________
      ## Build package off code repository
      
      ExternalProject_Add (pyqt
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	SVN_REPOSITORY ${PYQT_SVN}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/configure.py -d ${CMAKE_INSTALL_PREFIX}/lib/python -b ${CMAKE_INSTALL_PREFIX}/bin -p ${CMAKE_INSTALL_PREFIX}/plugins
	)
      
    else (PYQT_FROM_REPOS AND PYQT_SVN)
      
      ##______________________________________________________
      ## Build package off tar-archive
      
      ExternalProject_Add (pyqt
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	URL ${PYQT_SOURCE}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/configure.py -d ${CMAKE_INSTALL_PREFIX}/lib/python -b ${CMAKE_INSTALL_PREFIX}/bin -p ${CMAKE_INSTALL_PREFIX}/plugins
	)
      
    endif (PYQT_FROM_REPOS AND PYQT_SVN)
    
    ##________________________________________________________
    ## Update global configuration settings
    
    if (LUS_BINARY_DIR)
      ExternalProject_Add_Step (pyqt update_configure
	COMMAND ${CMAKE_COMMAND} -E remove CMake*
	COMMAND ${CMAKE_COMMAND} ..
	COMMENT "Updating configuration settings after installation of PyQt ..."
	DEPENDEES install
	DEPENDERS build
	WORKING_DIRECTORY ${LUS_BINARY_DIR}
	)
    endif (LUS_BINARY_DIR)
    
  else (PYTHON_EXECUTABLE)
    mesaage (WARNING "[PyQt] Unable to build package - missing Python executable!")
  endif (PYTHON_EXECUTABLE)
  
else (NOT PYQT_FOUND OR PYQT_FORCE_BUILD)
  
  add_custom_target (pyqt
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[PyQt] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT PYQT_FOUND OR PYQT_FORCE_BUILD)
