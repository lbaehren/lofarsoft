
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

cmake_minimum_required (VERSION 2.8.3)
cmake_policy (VERSION 2.8)
include (ExternalProject)

##__________________________________________________________
## Name of the project handled by CMake

project (FFTW)

##__________________________________________________________
## Package configuration/build options

option (FFTW_FORCE_BUILD     "Force build and installation of package?" YES )
option (FFTW_FROM_REPOS      "Install package from repository?"         NO  )
option (FFTW_MODIFIED_SOURCE "Use modified source archive?"             YES )

##__________________________________________________________
##  Package information

if (FFTW_MODIFIED_SOURCE)
  set (FFTW_VERSION          "3.1.2" )
  set (FFTW_SOURCE_DIRECTORY "fftw-${FFTW_VERSION}-modified" )
  set (FFTW_SOURCE_ARCHIVE   "${FFTW_SOURCE_DIRECTORY}.tgz" )
  set (FFTW_URL              "http://usg.lofar.org/svn/code/trunk/src/Pulsar/external/${FFTW_SOURCE_ARCHIVE}" )
  set (FFTW_SVN            "http://usg.lofar.org/svn/code/trunk/src/Pulsar/external/fftw" )
else (FFTW_MODIFIED_SOURCE)
  set (FFTW_VERSION        "3.3" )
  set (FFTW_SOURCE_ARCHIVE "fftw-${FFTW_VERSION}.tar.gz" )
  set (FFTW_URL            "ftp://ftp.fftw.org/pub/fftw/${FFTW_SOURCE_ARCHIVE}" )
  set (FFTW_SVN            "" )
endif (FFTW_MODIFIED_SOURCE)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT FFTW3_FOUND)
  find_package (FFTW3)
endif (NOT FFTW3_FOUND)

## CPU-dependent optimization

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
  set (FFTW_EXTRA_FLAGS "--enable-sse")
else (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set (FFTW_EXTRA_FLAGS "--enable-sse")
  else (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set (FFTW_EXTRA_FLAGS "--enable-altivec")
  endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (FFTW_FROM_REPOS)
  set (FFTW_FORCE_BUILD TRUE)
endif (FFTW_FROM_REPOS)

if (NOT FFTW3_FOUND OR FFTW_FORCE_BUILD)
  
  ##________________________________________________________
  ## Locate source file to build from
  
  find_file (FFTW_SOURCE ${FFTW_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT FFTW_SOURCE AND FFTW_URL)
    set (FFTW_SOURCE ${FFTW_URL})
  endif (NOT FFTW_SOURCE AND FFTW_URL)
    
  if (FFTW_FROM_REPOS AND FFTW_SVN)

    ##______________________________________________________
    ## Build package off code repository
    
    ExternalProject_Add (fftw
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${FFTW_SVN}
      BUILD_IN_SOURCE 1
      PATCH_COMMAND tar -xvzf ${FFTW_SOURCE_ARCHIVE}
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/${FFTW_SOURCE_DIRECTORY}/configure --prefix=${CMAKE_INSTALL_PREFIX} --enable-shared --enable-single ${FFTW_EXTRA_FLAGS}
      )
  
  else (FFTW_FROM_REPOS AND FFTW_SVN)

    ##______________________________________________________
    ## Build package off tar-archive
    
    ExternalProject_Add (fftw
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${FFTW_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} --enable-shared --enable-single ${FFTW_EXTRA_FLAGS}
      )
    
  endif (FFTW_FROM_REPOS AND FFTW_SVN)

  ##________________________________________________________
  ## Update global configuration settings
  
  if (PULSAR_BINARY_DIR)
    ExternalProject_Add_Step (fftw pulsar_update_cache
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ${PULSAR_SOURCE_DIR}
      COMMENT "Updating configuration settings after installation of FFTW3 ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${PULSAR_BINARY_DIR}
      )
  endif (PULSAR_BINARY_DIR)
  
else (NOT FFTW3_FOUND OR FFTW_FORCE_BUILD)
  
  add_custom_target (fftw
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[fftw] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT FFTW3_FOUND OR FFTW_FORCE_BUILD)
