
## -----------------------------------------------------------------------------
## Project name and version number number

project (wcslib)

set (wcslib_VERSION_MAJOR 4)
set (wcslib_VERSION_MINOR 2)
set (wcslib_VERSION "${wcslib_VERSION_MAJOR}.${wcslib_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

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
include (CheckLibraryExists)
include (CPack)

## -----------------------------------------------------------------------------
## Check for required external libraries

## CFITSIO --------------------------------------

find_path (fitsio_h fitsio.h ${include_locations})
find_library (libcfitsio cfitsio ${lib_locations})

## libm -----------------------------------------

find_library (libm m ${lib_locations})

## PGPlot ---------------------------------------

find_library (libpgplot pgplot ${lib_locations} PATH_SUFFIXES pgplot)
find_library (libcpgplot cpgplot ${lib_locations} PATH_SUFFIXES pgplot)

## Summary of the searches ----------------------

message (STATUS "CFITSIO headers  = ${fitsio_h}")
message (STATUS "CFITSIO library  = ${libcfitsio}")
message (STATUS "libm library     = ${libm}")
message (STATUS "PGPlot library   = ${libpgplot}")
message (STATUS "PGPlot C library = ${libcpgplot}")

## -----------------------------------------------------------------------------
## Subdirectories

subdirs (
  ${wcslib_SOURCE_DIR}/C
  ${wcslib_SOURCE_DIR}/FORTRAN
  ${wcslib_SOURCE_DIR}/pgsbox
)

## -----------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${wcslib_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package wcslib.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

