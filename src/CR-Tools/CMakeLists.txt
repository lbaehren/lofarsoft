
##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## Name of the project
project (CR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## User defined build/install options

option (CR_BUILD_TESTS "Build the test programs?" 1)
option (CR_ENABLE_TESTING "Build the test programs?" 1)
option (CR_DEBUGGING_MESSAGES "Print debugging information?" 0)
option (CR_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)
option (CR_GLISH_SUPPORT "Activate support for Glish applications?" 0)
option (CR_PYTHON_SUPPORT "Activate support for Glish applications?" 0)

## Handle option: testing mode on/off

if (CR_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (CR_ENABLE_TESTING)

## Handle option: verbose mode on/off

if (NOT CR_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY 1)
  set (CASA_FIND_QUIETLY 1)
  set (CASACORE_FIND_QUIETLY 1)
  set (CFITSIO_FIND_QUIETLY 1)
  set (FFTW3_FIND_QUIETLY 1)
  set (G2C_FIND_QUIETLY 1)
  set (GLISH_FIND_QUIETLY 1)
  set (GSL_FIND_QUIETLY 1)
  set (HDF5_FIND_QUIETLY 1)
  set (LAPACK_FIND_QUIETLY 1)
  set (PGPLOT_FIND_QUIETLY 1)
  set (POPT_FIND_QUIETLY 1)
  set (PostgreSQL_FIND_QUIETLY 1)
  set (ROOT_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
endif (NOT CR_VERBOSE_CONFIGURE)

## Handle option: Activate support for Glish applications on/off

# --> handled below

## -----------------------------------------------------------------------------
## Subdirectories

subdirs (
  implement
  apps
  )

if (CR_BUILD_TESTS)
  subdirs (test)
endif (CR_BUILD_TESTS)

## =============================================================================
##
##  External dependencies
##
## =============================================================================

find_library (libm m
  PATHS /usr/lib /usr/local/lib /sw/lib /opt/lib
  )

## Standard CMake modules for location of packages

find_package (Motif)
find_package (PNG)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## -----------------------------------------------------------------------------
## Locate additional CMake find scripts

find_path (cr_cmake FindBlitz.cmake FindCASA.cmake FindCFITSIO.cmake
  PATHS 
  ${CR_SOURCE_DIR}
  ${CR_SOURCE_DIR}/..
  ${CR_SOURCE_DIR}/../..
  ${CR_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (cr_cmake)
  ## Blitz++
  if (NOT HAVE_BLITZ)
    include (${cr_cmake}/FindBlitz.cmake)
  endif (NOT HAVE_BLITZ)
  if (HAVE_BLITZ)
    add_definitions (-DHAVE_BLITZ)
  endif (HAVE_BLITZ)
  ## CFITSIO
  if (NOT HAVE_CFITSIO)
    include (${cr_cmake}/FindCFITSIO.cmake)
  endif (NOT HAVE_CFITSIO)
  ## FFTW3
  if (NOT HAVE_FFTW3)
    include (${cr_cmake}/FindFFTW3.cmake)
  endif (NOT HAVE_FFTW3)
  ## Glish ---------------------------------------
  if (NOT HAVE_GLISH)
    include (${cr_cmake}/FindGlish.cmake)
  endif (NOT HAVE_GLISH)
  ## GNU Scientific Library (GSL) ----------------
  if (NOT HAVE_GSL)
    include (${cr_cmake}/FindGSL.cmake)
  endif (NOT HAVE_GSL)
  ## HDF5 ----------------------------------------
  if (NOT HAVE_HDF5)
    include (${cr_cmake}/FindHDF5.cmake)
  endif (NOT HAVE_HDF5)
  ## LAPACK
  if (NOT HAVE_LAPACK)
    include (${cr_cmake}/FindLAPACK.cmake)
  endif (NOT HAVE_LAPACK)
  ## -- PGPlot ---------------
  if (NOT HAVE_PGPLOT)
    include (${cr_cmake}/FindPGPLOT.cmake)
  endif (NOT HAVE_PGPLOT)
  ## -- popt -----------------
  if (NOT HAVE_POPT)
    include (${cr_cmake}/FindPOPT.cmake)
  endif (NOT HAVE_POPT)
  ## -- PostgreSQL -----------
  if (NOT HAVE_PostgreSQL)
    include (${cr_cmake}/FindPostgreSQL.cmake)
  endif (NOT HAVE_PostgreSQL)
  ## -- ROOT -----------------
  if (NOT HAVE_ROOT)
    include (${cr_cmake}/FindROOT.cmake)
  endif (NOT HAVE_ROOT)
  ## -- WCSLIB ---------------
  if (NOT HAVE_WCSLIB) 
    include (${cr_cmake}/FindWCS.cmake)
  endif (NOT HAVE_WCSLIB)
elseif (cr_cmake)
  message (SEND_ERROR "Unable to locate additional CMake scripts!")
endif (cr_cmake)

## Handle option: Activate support for Glish applications on/off

# If there is no Glish on the system, there is no use in trying to activate
# the support for Glish applications

if (NOT HAVE_GLISH)
  set (CR_GLISH_SUPPORT FALSE)
endif (NOT HAVE_GLISH)

if (CR_GLISH_SUPPORT)
  ## --- Support for Glish applications requires full CASA installation
  if (NOT HAVE_CASA)
    message (STATUS "[CR] Configuration to use CASA ...")
    include (${cr_cmake}/FindCASA.cmake)
  endif (NOT HAVE_CASA)
  ## Compiler flags for CASA
  if (HAVE_CASA)
    set (HAVE_CASACORE FALSE)
    add_definitions (-DHAVE_CASA)
  else (HAVE_CASA)
    message (STATUS "[CR] Failed to configure using CASA !")
  endif (HAVE_CASA)
  ## Compiler flags for Glish
  if (HAVE_GLISH)
    add_definitions (-DHAVE_GLISH)
  elseif (HAVE_GLISH)
    message (STATUS "[CR] Failed to locate Glish!")
    message (STATUS "     CR-Tools will be build without Glish Support!")
  endif (HAVE_GLISH)
else (CR_GLISH_SUPPORT)
  ## --- Without support for Glish applications we can/must use casacore
  message (STATUS "[CR] Configuration to use CASACORE ...")
  include (${cr_cmake}/FindCASACORE.cmake)
  if (HAVE_CASACORE)
    set (HAVE_CASA FALSE)
    add_definitions (-DHAVE_CASA)
    add_definitions (-DHAVE_CASACORE)
  endif (HAVE_CASACORE)
endif (CR_GLISH_SUPPORT)


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
  PATHS
  ${CR_SOURCE_DIR}/..
  ${CR_SOURCE_DIR}/../..
  PATH_SUFFIXES
  DAL/implement
  )

## ------------------------------------------------------------------------------
## Where to find the header files

include_directories (
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/apps
  ${CR_SOURCE_DIR}/test
  ${DAL_INCLUDES}
  ${PGPLOT_INCLUDES}
  )

## external components

if (BLITZ_INCLUDES)
  include_directories (${BLITZ_INCLUDES})
endif (BLITZ_INCLUDES)

if (CR_GLISH_SUPPORT)
  if (HAVE_CASA)
    include_directories (${CASA_INCLUDES})
  endif (HAVE_CASA)
else (CR_GLISH_SUPPORT)
  if (HAVE_CASACORE)
    include_directories (${CASACORE_INCLUDES})
  endif (HAVE_CASACORE)
endif (CR_GLISH_SUPPORT)

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
  ${CR_SOURCE_DIR}
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
message (STATUS "Glish ................. : ${HAVE_GLISH}")
message (STATUS "GSL ................... : ${HAVE_GSL}")
message (STATUS "HDF5 .................. : ${HAVE_HDF5}")
message (STATUS "LAPACK ................ : ${HAVE_LAPACK}")
message (STATUS "PGPLOT ................ : ${HAVE_PGPLOT}")
message (STATUS "POPT .................. : ${HAVE_POPT}")
message (STATUS "ROOT .................. : ${HAVE_ROOT}")
message (STATUS "WCSLIB ................ : ${HAVE_WCSLIB}")
