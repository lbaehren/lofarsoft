
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (SIP)

##__________________________________________________________
##  Package information

set (SIP_VERSION_MAJOR  "4"    )
set (SIP_VERSION_MINOR  "13"   )
set (SIP_VERSION        "${SIP_VERSION_MAJOR}.${SIP_VERSION_MINOR}" )
set (SIP_SOURCE_ARCHIVE "sip-${SIP_VERSION}.tar.gz" )
set (SIP_URL            "http://www.riverbankcomputing.com/static/Downloads/sip${SIP_VERSION_MAJOR}/${SIP_SOURCE_ARCHIVE}" )

##__________________________________________________________
## Package configuration/build options

option (SIP_FORCE_BUILD  "Force build and installation of package?" NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT PYTHON_EXECUTABLE)
  find_package (Python)
endif (NOT PYTHON_EXECUTABLE)

if (NOT SIP_FOUND)
  find_package (SIP)
endif (NOT SIP_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (NOT SIP_FOUND OR SIP_FORCE_BUILD)
  
  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (SIP_SOURCE ${SIP_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT SIP_SOURCE AND SIP_URL)
    set (SIP_SOURCE ${SIP_URL})
  endif (NOT SIP_SOURCE AND SIP_URL)
  
  ##________________________________________________________
  ## Build package off tar-archive
  
  ExternalProject_Add (sip
    PREFIX ${PROJECT_BINARY_DIR}
    DOWNLOAD_DIR download
    SOURCE_DIR source
    BUILD_IN_SOURCE 1
    URL ${SIP_SOURCE}
    CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/source/configure.py -b ${CMAKE_INSTALL_PREFIX}/bin -d ${CMAKE_INSTALL_PREFIX}/lib/python -e ${CMAKE_INSTALL_PREFIX}/include/sip -v ${CMAKE_INSTALL_PREFIX}/share/sip
    )
    
  ##________________________________________________________
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (sip update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of sip ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT SIP_FOUND OR SIP_FORCE_BUILD)
  
  add_custom_target (sip
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[sip] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT SIP_FOUND OR SIP_FORCE_BUILD)
