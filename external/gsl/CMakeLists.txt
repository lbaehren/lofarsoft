
## Name of the project
project (GSL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (GSL_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (GSL_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (GSL_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${GSL_SOURCE_DIR}
  ${GSL_SOURCE_DIR}/..
  ${GSL_SOURCE_DIR}/../..
  ${GSL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT GSL_VERBOSE_CONFIGURE)
  set (GSL_FIND_QUIETLY  YES )
endif (NOT GSL_VERBOSE_CONFIGURE)

include (FindGSL)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_GSL OR GSL_FORCE_BUILD)

  set (GSL_VERSION  "1.13" )
  set (GSL_SOURCE   "gsl-${GSL_VERSION}.tar.gz" )
  set (GSL_URL      "ftp://ftp.mirror.nl/pub/mirror/gnu/gsl/${GSL_SOURCE}" )
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_gsl_tgz ${GSL_SOURCE} ${GSL_SOURCE_DIR})

  if (NOT have_gsl_tgz)
    set (have_gsl_tgz ${GSL_URL})
  endif (NOT have_gsl_tgz)
  
  ExternalProject_Add (gsl
    PREFIX ${GSL_BINARY_DIR}
    DOWNLOAD_DIR ${GSL_BINARY_DIR}/download
    SOURCE_DIR ${GSL_BINARY_DIR}/source
    URL ${have_gsl_tgz}
    URL_MD5 d9fcfa367c44ab68a25b4edf34c3c5f7
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${GSL_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_GSL OR GSL_FORCE_BUILD)
  add_custom_target (gsl
    WORKING_DIRECTORY ${GSL_BINARY_DIR}
    COMMENT "[GSL] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_GSL OR GSL_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Configuration summary

if (GSL_VERBOSE_CONFIGURE) 

  message ("--------------------------------------------------")
  message ("   [GSL] Configuration summary.")
  message (" ")
  message (STATUS "C++ compiler    = ${CMAKE_CXX_COMPILER}")
  message (STATUS "BLAS library    = ${LAPACK_BLAS_LIBRARY}")
  message (STATUS "CBLAS library   = ${LAPACK_CBLAS_LIBRARY}")
  message (STATUS "LAPACK library  = ${LAPACK_LAPACK_LIBRARY}")
  message (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
  message (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
  message ("--------------------------------------------------")

endif (GSL_VERBOSE_CONFIGURE)
