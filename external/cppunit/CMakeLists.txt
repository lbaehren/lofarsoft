
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (CPPUNIT)

##__________________________________________________________
##  Package information

set (CPPUNIT_VERSION        "1.12.1" )
set (CPPUNIT_SOURCE_ARCHIVE "cppunit-${CPPUNIT_VERSION}.tar.gz" )
set (CPPUNIT_URL            "http://sourceforge.net/projects/cppunit/files/cppunit/${CPPUNIT_VERSION}/${CPPUNIT_SOURCE_ARCHIVE}" )
set (CPPUNIT_SVN            "https://cppunit.svn.sourceforge.net/svnroot/cppunit/trunk/cppunit" )

##__________________________________________________________
## Package configuration/build options

option (CPPUNIT_FORCE_BUILD  "Force build and installation of package?" NO )
option (CPPUNIT_FROM_REPOS   "Install package from repository?"         NO )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

find_package (CppUnit)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (CPPUNIT_FROM_REPOS)
  set (CPPUNIT_FORCE_BUILD TRUE)
endif (CPPUNIT_FROM_REPOS)

if (NOT CPPUNIT_FOUND OR CPPUNIT_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (CPPUNIT_SOURCE ${CPPUNIT_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT CPPUNIT_SOURCE AND CPPUNIT_URL)
    set (CPPUNIT_SOURCE ${CPPUNIT_URL})
  endif (NOT CPPUNIT_SOURCE AND CPPUNIT_URL)
    
  if (CPPUNIT_FROM_REPOS AND CPPUNIT_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (cppunit
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${CPPUNIT_SVN}
      BUILD_IN_SOURCE 1
      PATCH_COMMAND ${PROJECT_BINARY_DIR}/source/autogen.sh
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  
  else (CPPUNIT_FROM_REPOS AND CPPUNIT_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (cppunit
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${CPPUNIT_SOURCE}
      BUILD_IN_SOURCE 1
      PATCH_COMMAND patch < ${PROJECT_SOURCE_DIR}/cppunit.patch
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --disable-doxygen
      )
    
  endif (CPPUNIT_FROM_REPOS AND CPPUNIT_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (cppunit update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of cppunit ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT CPPUNIT_FOUND OR CPPUNIT_FORCE_BUILD)
  
  add_custom_target (cppunit
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[cppunit] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT CPPUNIT_FOUND OR CPPUNIT_FORCE_BUILD)
