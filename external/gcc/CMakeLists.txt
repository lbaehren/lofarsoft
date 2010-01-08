
## Name of the project
project (GCC)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (GCC_FORCE_BUILD       "Force rebuild and local installation?"     NO  )
option (GCC_CMAKE_BUILD       "Use CMake to configure and build Blitz++?" YES )
option (GCC_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (GCC_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${GCC_SOURCE_DIR}
  ${GCC_SOURCE_DIR}/..
  ${GCC_SOURCE_DIR}/../..
  ${GCC_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check build condition

find_program (gfortran_executable gfortran PATHS ${bin_locations})

if (not gfortran_executable)
  set (HAVE_GCC NO)
endif (not gfortran_executable)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_GCC OR GCC_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)

  set (GCC_VERSION 4.4.2)
  set (GCC_ARCHIVE gcc-${GCC_VERSION}.tar.gz)
  set (GCC_URL ftp://ftp.nluug.nl/mirror/languages/gcc/releases/gcc-${GCC_VERSION}/${GCC_ARCHIVE})

  set (GFORTRAN_ARCHIVE gcc-${GCC_VERSION}.tar.gz)
  set (GFORTRAN_URL ftp://ftp.nluug.nl/mirror/languages/gcc/releases/gcc-${GCC_VERSION}/${GFORTRAN_ARCHIVE})
  
  find_file (have_gcc_tgz ${GCC_ARCHIVE} ${GCC_SOURCE_DIR})
  find_file (have_gfortran_tgz ${GFORTRAN_ARCHIVE} ${GCC_SOURCE_DIR})
  
  if (have_gcc_tgz)
    ExternalProject_Add (gcc
      PREFIX ${GCC_BINARY_DIR}
      DOWNLOAD_DIR ${GCC_BINARY_DIR}
      SOURCE_DIR ${GCC_BINARY_DIR}
      URL ${have_gcc_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GCC_BINARY_DIR}/configure
      )
  else (have_gcc_tgz)
    ExternalProject_Add (gcc
      PREFIX ${GCC_BINARY_DIR}
      DOWNLOAD_DIR ${GCC_BINARY_DIR}
      SOURCE_DIR ${GCC_BINARY_DIR}
      URL ${GCC_URL}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${GCC_BINARY_DIR}/configure
      )
  endif (have_gcc_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_GCC OR GCC_FORCE_BUILD)
  message (STATUS "[gcc] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_GCC OR GCC_FORCE_BUILD)
