
##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 724 2007-09-10 10:17:21Z baehren                        $
##------------------------------------------------------------------------------

## Name of the project
project (DSP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## User defined build/install options

option (DSP_BUILD_TESTS "Build the test programs?" 1)
option (DSP_ENABLE_TESTING "Build the test programs?" 1)
option (DSP_DEBUGGING_MESSAGES "Print debugging information?" 0)
option (DSP_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)
option (DSP_PYTHON_SUPPORT "Activate support for Glish applications?" 0)

## Handle option: testing mode on/off

if (DSP_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (DSP_ENABLE_TESTING)

## Handle option: verbose mode on/off

if (NOT DSP_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY 1)
  set (CASA_FIND_QUIETLY 1)
  set (CASACORE_FIND_QUIETLY 1)
  set (CFITSIO_FIND_QUIETLY 1)
  set (FFTW3_FIND_QUIETLY 1)
  set (G2C_FIND_QUIETLY 1)
  set (GLISH_FIND_QUIETLY 1)
  set (GSL_FIND_QUIETLY 1)
  set (LAPACK_FIND_QUIETLY 1)
  set (PGPLOT_FIND_QUIETLY 1)
  set (POPT_FIND_QUIETLY 1)
  set (PostgreSQL_FIND_QUIETLY 1)
  set (ROOT_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
endif (NOT DSP_VERBOSE_CONFIGURE)

## Handle option: Activate support for Glish applications on/off

# --> handled below

## -----------------------------------------------------------------------------
## Subdirectories

subdirs (
  implement
  apps
  )

## =============================================================================
##
##  External dependencies
##
## =============================================================================

find_library (libm m PATHS /usr/lib /usr/local/lib /sw/lib)

## Standard CMake modules for location of packages

find_package (Motif)
find_package (PNG)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## -----------------------------------------------------------------------------
## Locate additional CMake find scripts

find_path (DSP_cmake FindBlitz.cmake FindCASA.cmake FindCFITSIO.cmake
  PATHS 
  ${DSP_SOURCE_DIR}
  ${DSP_SOURCE_DIR}/..
  ${DSP_SOURCE_DIR}/../..
  ${DSP_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (DSP_cmake)
  ## Blitz++
  if (NOT HAVE_BLITZ)
    include (${DSP_cmake}/FindBlitz.cmake)
  endif (NOT HAVE_BLITZ)
  if (HAVE_BLITZ)
    add_definitions (-DHAVE_BLITZ)
  endif (HAVE_BLITZ)
  ## CASACORE
  if (NOT HAVE_CASACORE)
    include (${DSP_cmake}/FindCASACORE.cmake)
  endif (NOT HAVE_CASACORE)
  ## CFITSIO
  if (NOT HAVE_CFITSIO)
    include (${DSP_cmake}/FindCFITSIO.cmake)
  endif (NOT HAVE_CFITSIO)
  ## FFTW3
  if (NOT HAVE_FFTW3)
    include (${DSP_cmake}/FindFFTW3.cmake)
  endif (NOT HAVE_FFTW3)
  ## GNU Scientific Library (GSL)
  if (NOT HAVE_GSL)
    include (${DSP_cmake}/FindGSL.cmake)
  endif (NOT HAVE_GSL)
  ## LAPACK
  if (NOT HAVE_LAPACK)
    include (${DSP_cmake}/FindLAPACK.cmake)
  endif (NOT HAVE_LAPACK)
  ## -- PGPlot ---------------
  if (NOT HAVE_PGPLOT)
    include (${DSP_cmake}/FindPGPLOT.cmake)
  endif (NOT HAVE_PGPLOT)
  ## -- WCSLIB ---------------
  if (NOT HAVE_WCSLIB) 
    include (${DSP_cmake}/FindWCS.cmake)
  endif (NOT HAVE_WCSLIB)
elseif (DSP_cmake)
  message (SEND_ERROR "Unable to locate additional CMake scripts!")
endif (DSP_cmake)

if (HAVE_CASA OR HAVE_CASACORE)
  add_definitions (
    -DWITH_CASA
    # -- compiler flags utilized in the source code --
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    )
  IF (APPLE)
    ## definition common to apple architectures
    add_definitions (
      -DAIPS_DARWIN
      )
  ELSE (APPLE)
    ADD_DEFINITIONS (-DAIPS_LINUX)
  ENDIF (APPLE)
  ## Platform test 32/64 bit ------------------------------
  set (CMAKE_SYSTEM_64BIT 0)
  if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
      if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
        set (CMAKE_SYSTEM_64BIT 1)
        add_definitions (-DAIPS_64B)
      endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
  endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
  if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
      set (CMAKE_SYSTEM_64BIT 1)
      add_definitions (-DAIPS_64B)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
  endif (APPLE)
endif (HAVE_CASA OR HAVE_CASACORE)

## ------------------------------------------------------------------------------
## Check for the presence of further USG comonents

find_path (DAL_INCLUDES dal.h lopesevent.h
  PATHS ${DSP_SOURCE_DIR}/.. ..
  PATH_SUFFIXES DAL/implement
  )

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  ${DSP_SOURCE_DIR}/implement
  ${DSP_SOURCE_DIR}/apps
  ${DSP_SOURCE_DIR}/test
  ${DAL_INCLUDES}
  ${PGPLOT_INCLUDES}
  )

## external components

if (BLITZ_INCLUDES)
  include_directories (${BLITZ_INCLUDES})
endif (BLITZ_INCLUDES)

if (HAVE_CASA)
  include_directories (${CASA_INCLUDES})
endif (HAVE_CASA)

if (HAVE_CASACORE)
  include_directories (${CASACORE_INCLUDES})
endif (HAVE_CASACORE)

if (CFITSIO_INCLUDES)
  include_directories (${CFITSIO_INCLUDES})
endif (CFITSIO_INCLUDES)

if (PGPLOT_INCLUDES)
  include_directories (${PGPLOT_INCLUDES})
endif (PGPLOT_INCLUDES)

if (GSL_INCLUDES)
  include_directories (${GSL_INCLUDES})
endif (GSL_INCLUDES)

if (PostgreSQL_INCLUDES)
  include_directories (${PostgreSQL_INCLUDES})
endif (PostgreSQL_INCLUDES)

if (ROOT_INCLUDES)
  include_directories (${ROOT_INCLUDES})
endif (ROOT_INCLUDES)

if (WCSLIB_INCLUDES)
  include_directories (${WCSLIB_INCLUDES})
endif (WCSLIB_INCLUDES)

## ------------------------------------------------------------------------------
## Verbose level

SET (CMAKE_VERBOSE_MAKEFILE 0)

## ------------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${DSP_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "[CR] Installation area located.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## ------------------------------------------------------------------------------
## Configuration summary

message (STATUS "CMAKE_SYSTEM .......... : ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR  : ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_SYSTEM_64BIT .....: ${CMAKE_SYSTEM_64BIT}")
message (STATUS "CMAKE_SYSTEM_BIG_ENDIAN : ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS "BLITZ ................. : ${HAVE_BLITZ}")
message (STATUS "CASA .................. : ${HAVE_CASA}")
message (STATUS "CASACORE .............. : ${HAVE_CASACORE}")
message (STATUS "CFITSIO ............... : ${HAVE_CFITSIO}")
message (STATUS "FFTW3 ................. : ${HAVE_FFTW3}")
message (STATUS "GSL ................... : ${HAVE_GSL}")
message (STATUS "LAPACK ................ : ${HAVE_LAPACK}")
message (STATUS "PGPLOT ................ : ${HAVE_PGPLOT}")
message (STATUS "WCSLIB ................ : ${HAVE_WCSLIB}")
