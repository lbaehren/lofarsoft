##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## Name of the project
project (CASA)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## =============================================================================
##
##  Configuration
##
## =============================================================================

## -----------------------------------------------------------------------------
## Options selectable by the user

option (CASA_BUILD_TESTS "Build the test programs?" 0)

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
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

include (CPack)
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

## check where to find the CMake scripts

find_path (cmake_modules FindFFTW3.cmake FindLAPACK.cmake
  PATHS 
  ${CASA_SOURCE_DIR}
  ${CASA_SOURCE_DIR}/..
  ${CASA_SOURCE_DIR}/../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (cmake_modules)
  include (${cmake_modules}/FindFFTW3.cmake)
  include (${cmake_modules}/FindG2C.cmake)
  include (${cmake_modules}/FindLAPACK.cmake)
  include (${cmake_modules}/FindPGPLOT.cmake)
  ## CFITSIO
  include (${cmake_modules}/FindCFITSIO.cmake)
  ## WCSLIB
  if (NOT HAVE_WCS)
  include (${cmake_modules}/FindWCS.cmake)
  endif (NOT HAVE_WCS)
endif (cmake_modules)

## WCSLIB -- in principle this can be a system-wide install, but in case none
##           such exists, we go checking whether we can build it from source

if (NOT HAVE_WCS)
  message (STATUS "Warning, no system-wide WCSLIB found - checking for source...")
  ## try to locate WCSLIB source
  find_file (WCS_CMAKE CMakeLists.txt
    PATHS ${CASA_SOURCE_DIR}/../wcslib
    )
  if (WCS_CMAKE)
    message (STATUS "Found WCSLIB source with CMake configuration.")
    set (WCS_INCLUDES ${CASA_SOURCE_DIR}/../wcslib/C)
  endif (WCS_CMAKE)
endif (NOT HAVE_WCS)

## -----------------------------------------------------------------------------
## Compiler settings
##
## The following command is used when building the CASA library from source,
## using the internal build environment:
##
## (a) Darwin - Mac OS X
## 
##  c++  -DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP
##       -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC 
##       -I/sw/share/casa/code/include -I. -O2 -fPIC  -fexceptions -pipe -Wall
##       -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment
##       -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC -c ???.cc;
##
## (b) Mac OS X (Darwin) - Intel
##
##  CPPDBG  += -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -DAIPS_ARRAY_INDEX_CHECK
##  CPPOPT  += -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP
##  CPPSTD  += -DAIPS_STDLIB
##
##  CC      := gcc
##  CDBG    += -DAIPS_SVID3 -DHAVE_LINUX_GLIBC -Wreturn-type -Wimplicit -fPIC
##  COPTLVL := -O2
##  COPT    += -DAIPS_SVID3 -DHAVE_LINUX_GLIBC -Wreturn-type -Wimplicit -fPIC
##
##  C++       := c++
##  C++DBG    += -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC
##  C++OPTLVL := -O2
##  C++OPT    += -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC
##
##  LDDBG     := -Xlinker -rpath -Xlinker $(AIPSARCH)/lib
##  LDOPT     := -s $(LDDBG)
##
## (c) Debian GNU/Linux
## 
##  CPPSTD   += -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB  -DAIPS_AUTO_STL
##              -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED
##
##  CC       := $(C++ROOT)/bin/gcc
##  CSTD     += -DHAVE_LINUX_GLIBC -Wall
##  COPTLVL  := -O2 -fno-strength-reduce
##
##  C++      := $(C++ROOT)/bin/g++ -Wno-deprecated
##  C++STD   += -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual
##              -Wmissing-prototypes -Wcast-align -Wno-comment -D_FILE_OFFSET_BITS=64
##              -D_LARGEFILE_SOURCE
##
##  which results in:
##
##  CPPOPT  = -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL
##            -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED  
##          = $(CPPSTD)
##
##  C++OPT  = -O3 -fPIC -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual
##            -Wmissing-prototypes -Wno-comment -D_FILE_OFFSET_BITS=64
##            -D_LARGEFILE_SOURCE
##  
## Optimized compile command within a package (e.g. casa/implement) is:
##
##  $(C++) $(CPPOPT) $(AIPSINCL) -I. $(C++OPT) -c ???.cc
##
## Thus by substitution of the above listed variables, we get
## 
##   $(C++ROOT)/bin/g++ -Wno-deprecated -DAIPS_NO_TEMPLATE_SRC -I.
##  

## Start with the generic information common to all the platforms

INCLUDE_DIRECTORIES (
  .
  ${CASA_SOURCE_DIR}
  ${CASA_SOURCE_DIR}/include
  ${CFITSIO_INCLUDES}
  ${WCS_INCLUDES}
  )

SET (CASA_OPTLEVEL "-O3")

## Platform-dependent compiler/linker settings

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
    ${CASA_OPTLEVEL}
    -fPIC
    -pipe
    -Wall
    -Wno-comment
    -fexceptions
    -Wcast-align
    -Wno-non-template-friend
    -Woverloaded-virtual
    )
ENDIF (UNIX)

## Now that all the platform-specific settings have been done, we can assign
## the variables internally used by CMake

SET (CMAKE_MODULE_LINKER_FLAGS "${CASA_LDSTD}")
SET (CMAKE_C_FLAGS "${CASA_OPTLEVEL} ${CASA_CXXSTD}")

## -----------------------------------------------------------------------------
## List of sub-directories

## list of CASA modules to be build

subdirs (
  casa
  scimath
  tables
  measures
  ms
  lattices
  fits
  coordinates
  )

## =============================================================================
##
##  Installation
##
## =============================================================================

find_path (prefix release_area.txt
  PATHS
  ${CASA_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package CASA.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## =============================================================================
##
##  Summary of settings
##
## =============================================================================

message (STATUS "CASA_SOURCE_DIR ......... = ${CASA_SOURCE_DIR}")
message (STATUS "CMAKE_SYSTEM_NAME ....... = ${CMAKE_SYSTEM_NAME}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR .. = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_C_FLAGS ........... = ${CMAKE_C_FLAGS}")
message (STATUS "CMAKE_CXX_FLAGS ......... = ${CMAKE_CXX_FLAGS}")
message (STATUS "CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")
message (STATUS "CMAKE_INSTALL_PREFIX .... = ${CMAKE_INSTALL_PREFIX}")

message (STATUS "CFITSIO_INCLUDES ........ = ${CFITSIO_INCLUDES}")
message (STATUS "CFITSIO_LIBRARIES ....... = ${CFITSIO_LIBRARIES}")
message (STATUS "FFTW3_INCLUDES .......... = ${FFTW3_INCLUDES}")
message (STATUS "FFTW3_LIBRARIES ......... = ${FFTW3_LIBRARIES}")
message (STATUS "WCS_INCLUDES ............ = ${WCS_INCLUDES}")
message (STATUS "WCS_LIBRARIES ........... = ${WCS_LIBRARIES}")
