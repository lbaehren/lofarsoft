
cmake_minimum_required (VERSION 2.8)
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

##__________________________________________________________
## Name of the project handled by CMake

project (CFITSIO)

##__________________________________________________________
##  Package information

set (CFITSIO_VERSION        "3140" )
set (CFITSIO_SOURCE_ARCHIVE "cfitsio${CFITSIO_VERSION}.tar.gz" )
set (CFITSIO_URL            "ftp://heasarc.gsfc.nasa.gov/software/fitsio/c/${CFITSIO_SOURCE_ARCHIVE}" )
set (CFITSIO_SVN            "" )

##__________________________________________________________
## Package configuration/build options

option (CFITSIO_FORCE_BUILD       "Force build and installation of package?" NO  )
option (CFITSIO_FROM_REPOS        "Install package from repository?"         NO  )
option (CFITSIO_enable_lfs        "Enable large file system support?"        YES )
option (CFITSIO_enable_reentrant  "Enable reentrant multithreading?"         NO  )
option (CFITSIO_enable_hera       "Build for HERA project?"                  NO  )

if (CFITSIO_enable_lfs)
  set (cfitsio_enable_lfs "--enable-lfs")
else (CFITSIO_enable_lfs)
  set (cfitsio_enable_lfs "--disable-lfs")
endif (CFITSIO_enable_lfs)

if (CFITSIO_enable_hera)
  set (cfitsio_enable_hera "--enable-hera")
else (CFITSIO_enable_hera)
  set (cfitsio_enable_hera "--disable-hera")
endif (CFITSIO_enable_hera)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT CFITSIO_FOUND)
  find_package (CFITSIO)
endif (NOT CFITSIO_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (CFITSIO_FROM_REPOS)
  set (CFITSIO_FORCE_BUILD TRUE)
endif (CFITSIO_FROM_REPOS)

if (NOT CFITSIO_FOUND OR CFITSIO_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (CFITSIO_SOURCE ${CFITSIO_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT CFITSIO_SOURCE AND CFITSIO_URL)
    set (CFITSIO_SOURCE ${CFITSIO_URL})
  endif (NOT CFITSIO_SOURCE AND CFITSIO_URL)
    
  if (CFITSIO_FROM_REPOS AND CFITSIO_SVN)

    ## Build package off code repository
    
    ExternalProject_Add (cfitsio
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${CFITSIO_SVN}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} ${cfitsio_enable_lfs} ${cfitsio_enable_hera}
      )
  
  else (CFITSIO_FROM_REPOS AND CFITSIO_SVN)

    ## Build package off tar-archive
    
    ExternalProject_Add (cfitsio
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${CFITSIO_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX} ${cfitsio_enable_lfs} ${cfitsio_enable_hera}
      )
    
  endif (CFITSIO_FROM_REPOS AND CFITSIO_SVN)

  ## Update global configuration settings
  
  if (LUS_BINARY_DIR)
    ExternalProject_Add_Step (cfitsio update_configure
      COMMAND ${CMAKE_COMMAND} -E remove CMake*
      COMMAND ${CMAKE_COMMAND} ..
      COMMENT "Updating configuration settings after installation of CFITSIO ..."
      DEPENDEES install
      DEPENDERS build
      WORKING_DIRECTORY ${LUS_BINARY_DIR}
      )
  endif (LUS_BINARY_DIR)
  
else (NOT CFITSIO_FOUND OR CFITSIO_FORCE_BUILD)
  
  add_custom_target (cfitsio
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[CFITSIO] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT CFITSIO_FOUND OR CFITSIO_FORCE_BUILD)
