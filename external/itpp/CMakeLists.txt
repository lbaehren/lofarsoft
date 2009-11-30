
## Name of the project
project (ITPP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ITPP_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (ITPP_CMAKE_BUILD       "Use CMake to configure and build IT++?"   YES )
option (ITPP_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (ITPP_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ITPP_SOURCE_DIR}
  ${ITPP_SOURCE_DIR}/..
  ${ITPP_SOURCE_DIR}/../..
  ${ITPP_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT ITPP_VERBOSE_CONFIGURE)
  set (FFTW3_FIND_QUIETLY  YES )
  set (LAPACK_FIND_QUIETLY  YES )
endif (NOT ITPP_VERBOSE_CONFIGURE)

include (FindFFTW3)
include (FindLAPACK)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ITPP OR ITPP_FORCE_BUILD)
  
  ## create script to run IT++ configuration
  
  if (HAVE_FFTW3)
    set (WITH_FFT ${FFTW3_LIBRARIES})
    set (WITH_FFTW_INCLUDES ${FFTW3_INCLUDES})
  else (HAVE_FFTW3)
    set (WITH_FFT "no")
    set (WITH_FFTW_INCLUDES "no")
  endif (HAVE_FFTW3)
  
  if (LAPACK_BLAS_LIBRARY)
    set (WITH_BLAS ${LAPACK_BLAS_LIBRARY})
  else (LAPACK_BLAS_LIBRARY)
    set (WITH_BLAS "no")
  endif (LAPACK_BLAS_LIBRARY)

  ## Create wrapper for the configure command
  
  configure_file (${ITPP_SOURCE_DIR}/run_configure.in
    ${ITPP_BINARY_DIR}/run_configure
    )

  file(
    COPY ${ITPP_BINARY_DIR}/run_configure
    DESTINATION ${ITPP_BINARY_DIR}/itpp
    FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ GROUP_EXECUTE GROUP_READ
    )
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_itpp_tgz itpp-4.0.6.tar.gz  ${ITPP_SOURCE_DIR})
  
  if (have_itpp_tgz)
    ExternalProject_Add (itpp
      PREFIX ${ITPP_BINARY_DIR}
      DOWNLOAD_DIR ${ITPP_BINARY_DIR}/itpp
      SOURCE_DIR ${ITPP_BINARY_DIR}/itpp
      URL ${have_itpp_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${ITPP_BINARY_DIR}/itpp/run_configure
      )
  else (have_itpp_tgz)
    status (WARNING "[IT++] Unable to build project - no sources found!")
  endif (have_itpp_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ITPP OR ITPP_FORCE_BUILD)
  message (STATUS "[IT++] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_ITPP OR ITPP_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Configuration summary

if (ITPP_VERBOSE_CONFIGURE) 

  message ("--------------------------------------------------")
  message ("   [IT++] Configuration summary.")
  message (" ")
  message (STATUS "C++ compiler    = ${CMAKE_CXX_COMPILER}")
  message (STATUS "BLAS library    = ${LAPACK_BLAS_LIBRARY}")
  message (STATUS "CBLAS library   = ${LAPACK_CBLAS_LIBRARY}")
  message (STATUS "LAPACK library  = ${LAPACK_LAPACK_LIBRARY}")
  message (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
  message (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
  message ("--------------------------------------------------")

endif (ITPP_VERBOSE_CONFIGURE)
