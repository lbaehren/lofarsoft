
## Name of the project
project (ITPP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

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
  
  ## ------------------------------------------------------------------------------
  ## 

  set (ITPP_VERSION "4.0.6")
  set (ITPP_DIRECTORY "itpp-${ITPP_VERSION}")
  set (ITPP_TAR_ARCHIVE "${ITPP_DIRECTORY}.tar.gz")
  
  find_file (have_itpp_tgz ${ITPP_TAR_ARCHIVE} ${ITPP_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})
  
  if (have_itpp_tgz AND tar_executable)
    
    ## expand the tar-archive
    if (ITPP_VERBOSE_BUILD)
      add_custom_target (itpp_expand
	COMMAND ${tar_executable} -xvzf ${have_itpp_tgz}
	WORKING_DIRECTORY ${ITPP_BINARY_DIR}
	COMMENT "[IT++] Expanding tar-archive ..."
	)
    else (ITPP_VERBOSE_BUILD)
      add_custom_target (itpp_expand
	COMMAND ${tar_executable} -xzf ${have_itpp_tgz}
	WORKING_DIRECTORY ${ITPP_BINARY_DIR}
	COMMENT "[IT++] Expanding tar-archive ..."
	)
    endif (ITPP_VERBOSE_BUILD)
    
    ## configure the package
    if (HAVE_FFTW3)
      add_custom_target (itpp_configure
	COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --with-fft=${FFTW3_LIBRARIES} --with-fft-include=${FFTW3_INCLUDES}
	WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
	COMMENT "[IT++] Configuring package ..."
	)
      if (LAPACK_BLAS_LIBRARY)
	add_custom_target (itpp_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --with-fft=${FFTW3_LIBRARIES} --with-fft-include=${FFTW3_INCLUDES} --with-blas=${LAPACK_BLAS_LIBRARY}
	  WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
	  COMMENT "[IT++] Configuring package ..."
	  )
      else (LAPACK_BLAS_LIBRARY)
	add_custom_target (itpp_configure
	  COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --with-fft=${FFTW3_LIBRARIES} --with-fft-include=${FFTW3_INCLUDES} --without-blas
	  WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
	  COMMENT "[IT++] Configuring package ..."
	  )
      endif (LAPACK_BLAS_LIBRARY)
    else (HAVE_FFTW3)
      add_custom_target (itpp_configure
	COMMAND ./configure --prefix=${USG_INSTALL_PREFIX} --without-fft
	WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
	COMMENT "[IT++] Configuring package ..."
	)
    endif (HAVE_FFTW3)
    
    add_dependencies (itpp_configure itpp_expand)
    
    ## build the package
    add_custom_target (itpp_build
      COMMAND make
      WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
      COMMENT "[IT++] Starting build ..."
      )
    add_dependencies (itpp_build itpp_configure)
    
    ## build and install the package
    add_custom_target (install
      COMMAND make install
      COMMAND rm -rf ${ITPP_BINARY_DIR}/*
      WORKING_DIRECTORY ${ITPP_BINARY_DIR}/${ITPP_DIRECTORY}
      COMMENT "[IT++] Installing package ..."
      )
    add_dependencies (install itpp_build)
    
  else (have_itpp_tgz AND tar_executable)
    message (STATUS "[IT++] Unable to install - missing tar-archive!")
  endif (have_itpp_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ITPP OR ITPP_FORCE_BUILD)
  message (STATUS "Found system-wide installation of ITPP; skipping rebuild!")
endif (NOT HAVE_ITPP OR ITPP_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Configuration summary

message ("--------------------------------------------------")
message ("   [IT++] Configuration summary.")
message (" ")
message (STATUS "BLAS library    = ${LAPACK_BLAS_LIBRARY}")
message (STATUS "CBLAS library   = ${LAPACK_CBLAS_LIBRARY}")
message (STATUS "LAPACK library  = ${LAPACK_LAPACK_LIBRARY}")
message (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
message (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
message ("--------------------------------------------------")
