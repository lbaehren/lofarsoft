
## ------------------------------------------------------------------------------
##
## Main CMake configuration file for building the "casacore" package.
##
## Pending issues:
##  - processing of files via flex and bison in "tables"
##  - fortran directory of "scimath"
##
## ------------------------------------------------------------------------------

message (STATUS "[CMake package configuration] casacore")

project (casacore)

set (casacore_packages
  casa
  components
  coordinates
  fits
  images
  lattices
  measures
  mirlib
  ms
  msfits
  msvis
  scimath
  tables
  )

subdirs (${casacore_packages})

include_directories (${casacore_packages})

## ------------------------------------------------------------------------------
## Dependency of the libraries

# casa        : --
# tables      : casa
# mirlib      : casa
# scimath     : casa
# measures    : tables scimath
# fits        : measures
# coordinates : fits
# components  : coordinates
# lattices    : tables scimath
# ms          : measures
# images      : components mirlib
# msfits      : ms fits
# msvis       : ms

## -----------------------------------------------------------------------------
## Check for the platform

IF (UNIX)
  IF (APPLE)
    SET (AIPSARCH "darwin")
    SET (AIPS_DARWIN 1)
  ELSE (APPLE)
    SET (AIPSARCH "linux")
    SET (AIPS_DARWIN 0)
  ENDIF (APPLE)
ENDIF (UNIX)

## -----------------------------------------------------------------------------
## Check for additional tools which are required

find_file (bison_bin bison /usr/bin /usr/local/bin /sw/bin)
find_file (flex_bin flex /usr/bin /usr/local/bin /sw/bin)

## -----------------------------------------------------------------------------
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

include (CheckIncludeFiles)
include (CheckLibraryExists)

find_path (HAVE_STRING_H
  string.h
  PATHS /usr/include /usr/local/include /sw/include
#  PATH_SUFFIXES c++ c++/4.0.0 g++
  )

## -----------------------------------------------------------------------------
## Required external libraries

find_package (Motif)
find_package (PNG)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## --- libm --------------------------------------

find_library (libm m
  PATHS
  /usr/lib
  /usr/local/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  /sw/lib
  )

if (libm)
  message (STATUS "Found LIBM: ${libm}")
endif (libm)

## -----------------------------------------------------------------------------
## Compiler and Linker options

IF (UNIX)
  add_definitions (
    ## CPPOPT
    -DAIPS_STDLIB
    -DAIPS_NO_TEMPLATE_SRC
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    ## LDOPT
##    -s -Xlinker -rpath -Xlinker
    )
  set (CASA_LDSTD "-Xlinker -rpath -Xlinker")
  IF (APPLE)
    ADD_DEFINITIONS (
      -DAIPS_DARWIN
      -DAIPS_BIG_ENDIAN
      -DNATIVE_EXCP
      -D_FILE_OFFSET_BITS=64
      -D_LARGEFILE_SOURCE
      )
    SET (CASA_CXXSTD "${CASA_CXXSTD} -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC")
  ELSE (APPLE)
    ADD_DEFINITIONS (
      -DAIPS_LINUX
      -DAIPS_LITTLE_ENDIAN
      -D_LARGEFILE_SOURCE
      )
  ENDIF (APPLE)
  ADD_DEFINITIONS (
    -O2
    -fPIC
    -pipe
    -Wall
    -Wno-comment
    -fexceptions
    -Wcast-align
#    -Wno-non-template-friend
#    -Woverloaded-virtual
    )
ENDIF (UNIX)

## ------------------------------------------------------------------------------
## Installation

include (CPack)

find_path (prefix release_area.txt
  PATHS
  ${casacore_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package casacore.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)
