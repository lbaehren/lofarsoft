
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (BOOST)

##__________________________________________________________
##  Package information

set (BOOST_VERSION_MAJOR  "1" )
set (BOOST_VERSION_MINOR  "47" )
set (BOOST_VERSION_PATCH  "0" )
set (BOOST_VERSION        "${BOOST_VERSION_MAJOR}.${BOOST_VERSION_MINOR}.${BOOST_VERSION_PATCH}" )
set (BOOST_SOURCE_ARCHIVE "boost_${BOOST_VERSION_MAJOR}_${BOOST_VERSION_MINOR}_${BOOST_VERSION_PATCH}.tar.bz2" )
set (BOOST_URL            "http://downloads.sourceforge.net/project/boost/boost/${BOOST_VERSION}/${BOOST_SOURCE_ARCHIVE}" )
set (BOOST_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (BOOST_FORCE_BUILD  "Force build and installation of package?" NO )
option (BOOST_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT BOOST_FOUND)
  find_package (Boost)
endif (NOT BOOST_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (BOOST_FROM_REPOS)
  set (BOOST_FORCE_BUILD TRUE)
endif (BOOST_FROM_REPOS)

if (NOT BOOST_FOUND OR BOOST_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (BOOST_SOURCE ${BOOST_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT BOOST_SOURCE AND BOOST_URL)
    set (BOOST_SOURCE ${BOOST_URL})
  endif (NOT BOOST_SOURCE AND BOOST_URL)
    
  if (BOOST_FROM_REPOS AND BOOST_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (boost
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${BOOST_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/bootstrap.sh --prefix=${CMAKE_INSTALL_PREFIX}
      BUILD_COMMAND ${PROJECT_BINARY_DIR}/source/bjam
      INSTALL_COMMAND ${PROJECT_BINARY_DIR}/source/bjam install --prefix=${LUS_INSTALL_PREFIX}
      )
  
  else (BOOST_FROM_REPOS AND BOOST_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (boost
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${BOOST_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/bootstrap.sh --prefix=${CMAKE_INSTALL_PREFIX}
      BUILD_COMMAND ${PROJECT_BINARY_DIR}/source/bjam
      INSTALL_COMMAND ${PROJECT_BINARY_DIR}/source/bjam install --prefix=${LUS_INSTALL_PREFIX}
      )
    
  endif (BOOST_FROM_REPOS AND BOOST_SVN)
  
  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (boost update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of boost ..."
      DEPENDEES install
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT BOOST_FOUND OR BOOST_FORCE_BUILD)
  
  add_custom_target (boost
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[boost] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT BOOST_FOUND OR BOOST_FORCE_BUILD)
