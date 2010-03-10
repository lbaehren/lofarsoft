
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

  set (gsl_version     "1.13")
  set (gsl_tar_archive "gsl-${gsl_version}.tar.gz")
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_gsl_tgz ${gsl_tar_archive}  ${GSL_SOURCE_DIR})
  
  if (have_gsl_tgz)
    ExternalProject_Add (gsl
      PREFIX ${GSL_BINARY_DIR}
      DOWNLOAD_DIR ${GSL_BINARY_DIR}
      SOURCE_DIR ${GSL_BINARY_DIR}
      URL ${have_gsl_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GSL_BINARY_DIR}/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  else (have_gsl_tgz)
    ExternalProject_Add (gsl
      PREFIX ${GSL_BINARY_DIR}
      DOWNLOAD_DIR ${GSL_BINARY_DIR}
      SOURCE_DIR ${GSL_BINARY_DIR}
      URL ftp://ftp.mirror.nl/pub/mirror/gnu/gsl/${GSL_TAR_ARCHIVE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GSL_BINARY_DIR}/configure --prefix=${CMAKE_INSTALL_PREFIX}
      )
  endif (have_gsl_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_GSL OR GSL_FORCE_BUILD)
  message (STATUS "[GSL] Found system-wide installation; skipping rebuild!")
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
