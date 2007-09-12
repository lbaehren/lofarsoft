##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

set (WCSLIB_VERSION_MAJOR 4)
set (WCSLIB_VERSION_MINOR 2)
set (WCSLIB_VERSION "${WCSLIB_VERSION_MAJOR}.${WCSLIB_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  ${casacore_SOURCE_DIR}/../release/bin
  ${casacore_SOURCE_DIR}/../../release/bin
  )

set (include_locations
  /usr/include
  /usr/local/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/c++/4.0.0
  /sw/include
  /sw/lib/gcc4.2/include/c++/4.2.1
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
)

include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CPack)

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (WCSLIB_cmake FindCFITSIO.cmake FindG2C.cmake FindLAPACK.cmake
  PATHS 
  ${WCSLIB_SOURCE_DIR}
  ${WCSLIB_SOURCE_DIR}/..
  ${WCSLIB_SOURCE_DIR}/../..
  ${WCSLIB_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## CFITSIO --------------------------------------

find_path (fitsio_h fitsio.h ${include_locations})
find_library (libcfitsio cfitsio ${lib_locations})

## libm -----------------------------------------

find_library (libm m ${lib_locations})

## PGPlot ---------------------------------------

find_library (libpgplot pgplot ${lib_locations} PATH_SUFFIXES pgplot)
find_library (libcpgplot cpgplot ${lib_locations} PATH_SUFFIXES pgplot)

## -----------------------------------------------------------------------------
## Check for additional tools which are required

find_program (flex_bin flex
  PATHS ${bin_locations}
)

## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (${WCSLIB_SOURCE_DIR}/C)
add_subdirectory (${WCSLIB_SOURCE_DIR}/FORTRAN)
add_subdirectory (${WCSLIB_SOURCE_DIR}/pgsbox)

## -----------------------------------------------------------------------------
## Installation

find_path (WCSLIB_INSTALL_PREFIX release_area.txt
  PATHS
  ${WCSLIB_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (WCSLIB_INSTALL_PREFIX)
  message (STATUS "Installation area located for package WCSLIB.")
  set (CMAKE_INSTALL_PREFIX ${WCSLIB_INSTALL_PREFIX}) 
endif (WCSLIB_INSTALL_PREFIX)

## -----------------------------------------------------------------------------
## Summary of settings

message (STATUS "CFITSIO headers ............... = ${fitsio_h}")
message (STATUS "CFITSIO library ............... = ${libcfitsio}")
message (STATUS "libm library .................. = ${libm}")
message (STATUS "PGPlot library ................ = ${libpgplot}")
message (STATUS "PGPlot C library .............. = ${libcpgplot}")
message (STATUS "Flex lexical analyzer generator = ${flex_bin}")
