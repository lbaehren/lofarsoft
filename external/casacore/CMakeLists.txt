##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
##
## Main CMake configuration file for building the "casacore" package.
##
## Pending issues:
##  - "libfits" does not yet build (on PPC Mac)
##  - "tVanVleck" does not link due to missing f2c symbols
##
## -----------------------------------------------------------------------------

message (STATUS "[CMake package configuration] casacore")

project (casacore)

## Dependency of the packages in casacore

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

set (casacore_packages
  casa
  tables
  mirlib
  scimath
  measures
  fits
  coordinates
  components
  lattices
  ms
  images
  msfits
  msvis
  )

## -----------------------------------------------------------------------------
## Options selectable by the user

option (casacore_tests
  "Build the test programs?"
  0
  )

option (CMAKE_VERBOSE_MAKEFILE
  "Verbose makefile?"
  0
  )

## -----------------------------------------------------------------------------
## Additional required CMake functionality

include (CPack)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckTypeSize)

## -----------------------------------------------------------------------------
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

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
  /sw/include
  /opt/casa/local/include
  ${casacore_SOURCE_DIR}/../release/include
  ${casacore_SOURCE_DIR}/../../release/include
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  ${casacore_SOURCE_DIR}/../release/lib
  ${casacore_SOURCE_DIR}/../../release/lib
  )

check_include_files (ctype.h HAVE_CTYPE_H)
check_include_files (fcntl.h HAVE_FCNTL_H)
check_include_files (string.h HAVE_STRING_H)
check_include_files (unistd.h HAVE_UNISTD_H)

check_type_size ("float" SIZEOF_FLOAT)
check_type_size ("double" SIZEOF_DOUBLE)

## -----------------------------------------------------------------------------
## Required external libraries

find_package (Motif)
find_package (PNG)
find_package (X11)
find_package (ZLIB)

include (./FindG2C.cmake)
include (./FindLAPACK.cmake)

## --- libm -----------------------------------------------

find_library (libm m PATHS ${lib_locations})

## --- CFITSIO --------------------------------------------

set (HAVE_CFITSIO 1)

find_path (CFITSIO_INCLUDES fitsio.h fitsio2.h
  PATHS ${include_locations}
  PATH_SUFFIXES cfitsio fitsio
)

find_library (CFITSIO_LIBRARIES cfitsio
  PATHS ${lib_locations}
  PATH_SUFFIXES cfitsio fitsio
)

if (NOT CFITSIO_INCLUDES)
  set (HAVE_CFITSIO 0)
endif (NOT CFITSIO_INCLUDES)

if (NOT CFITSIO_LIBRARIES)
  set (HAVE_CFITSIO 0)
endif (NOT CFITSIO_LIBRARIES)

if (NOT HAVE_CFITSIO)
  message (SEND_ERROR "Unable to find systm-wide installation of CFITSIO!")
endif (NOT HAVE_CFITSIO)

## --- WCSLIB ---------------------------------------------
##
## As the source code of WCSLIB is in the USG code repository,
## we do have the possiblity to build the library from there
## in case no system-wide installation can be found; this of
## course only will be possible, if the "externa" directory
## has been checked out completely.

set (HAVE_WCSLIB 1)

find_library (WCSLIB_LIBRARIES wcs ${lib_locations})

find_path (WCSLIB_INCLUDES wcs.h wcshdr.h
  PATHS ${include_locations}
  PATH_SUFFIXES C wcs wcslib wcslib-4.2
)

if (NOT WCSLIB_INCLUDES)
  set (HAVE_WCSLIB 0)
endif (NOT WCSLIB_INCLUDES)

if (NOT WCSLIB_LIBRARIES)
  set (HAVE_WCSLIB 0)
endif (NOT WCSLIB_LIBRARIES)

if (NOT HAVE_WCSLIB)
  message (SEND_ERROR "WCSLIB_INCLUDES  = ${WCSLIB_INCLUDES}")
  message (SEND_ERROR "WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")
  message (FATAL_ERROR "Unable to find systm-wide installation of WCSLIB!")
	## try to locate the source directory first
	find_path (WCSLIB_INCLUDES wcs.h wcshdr.h 
	  PATHS
	  ${casacore_SOURCE_DIR}/../wcslib-4.2
	  ${casacore_SOURCE_DIR}/../wcslib
	  PATH_SUFFIXES 
	  C
	)
	## if the source directory could be located, trigger build of WCSLIB
	if (WCSLIB_INCLUDES)
	  ## create a symbolic link within the casacore directory
      execute_process (
        WORKING_DIRECTORY ${casacore_SOURCE_DIR}
        COMMAND ln -fs ${WCSLIB_INCLUDES}/.. wcslib
        TIMEOUT 20
        ERROR_VARIABLE ln_error
        OUTPUT_QUIET
      )
	  subdirs (wcslib)
	elseif (WCSLIB_INCLUDES)
	  message (SEND_ERROR "Unable to locate WCSLIB source directory!")
	endif (WCSLIB_INCLUDES)
endif (NOT HAVE_WCSLIB)

## -----------------------------------------------------------------------------
## Check for additional tools which are required

find_program (bison_bin bison
  PATHS ${bin_locations}
  )

find_program (flex_bin flex
  PATHS ${bin_locations}
)

find_program (uname_bin uname ${bin_locations})

## -----------------------------------------------------------------------------
## Compiler and Linker options
## 
## Examples from the configuration and build using SCons:
##
## (a) Apple Mac OS-X PPC
##
##  [libcasa]
##  g++ -o "build_darwin_Power Macintosh/opt/aips.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. casa/aips.cc
##  g++ -o "build_darwin_Power Macintosh/opt/Exceptions/Error2.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. casa/Exceptions/Error2.cc
##  [libtables]
##  g++ -o "build_darwin_Power Macintosh/opt/Tables/TaQLResult.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. -I/Volumes/UNIX/casacore/stage/include/casacore "-Ibuild_darwin_Power Macintosh/opt/Tables" tables/Tables/TaQLResult.cc
##  g++ -o "build_darwin_Power Macintosh/opt/Tables/DataManager.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. -I/Volumes/UNIX/casacore/stage/include/casacore "-Ibuild_darwin_Power Macintosh/opt/Tables" tables/Tables/DataManager.cc
##  [libmir]
##  gcc -o "build_darwin_Power Macintosh/opt/dio.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. dio.c
##  gcc -o "build_darwin_Power Macintosh/opt/bug.o" -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_DARWIN -DAIPS_NO_LEA_MALLOC -O2 -I. bug.c
##  [libscimath]
##  [measures]
##  [fits]
##  [coordinates]
##  [components]
##
## (b) Debian GNU Linux
##
##  [libcasa]
##  g++ -o build_linux_i686/opt/aips.o -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -O2 -I. casa/aips.cc
##  g++ -o build_linux_i686/opt/Exceptions/Error2.o -c -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -O2 -I. casa/Exceptions/Error2.cc
##
## The following is for fixing compiler settings for libfits
##
## /usr/bin/c++ -DAIPS_STDLIB -DAIPS_NO_TEMPLATE_SRC -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fPIC -pipe -Wall -Wno-comment -fexceptions -Wcast-align -Wno-non-template-friend -Woverloaded-virtual -o fits/implement/CMakeFiles/fits.dir/FITS/fits.o -c /Users/lars/Code/lofar/usg/external/casa/fits/implement/FITS/fits.cc
## /usr/bin/c++ -DAIPS_STDLIB -DAIPS_NO_TEMPLATE_SRC -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O2 -pipe -fPIC -Wall -Wno-comment -fexceptions -Wno-non-template-friend -Woverloaded-virtual -o fits/fits/CMakeFiles/fits.dir/FITS/BasicFITS.o -c /Users/lars/Code/lofar/usg/external/casacore/fits/fits/FITS/BasicFITS.cc

## --- Location of the header files --------------

include_directories (
  .
  ${casacore_packages}
  ${casacore_SOURCE_DIR}/fits
  ${casacore_SOURCE_DIR}/images/images/Images
  ${casacore_SOURCE_DIR}/ms/ms/MeasurementSets
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  ${CFITSIO_INCLUDES}
  ${G2C_INCLUDES}
  )

## --- Compiler settings -------------------------

IF (UNIX)
  add_definitions (
    # -- optimization --
    -O2
    -fPIC
    -pipe
    # -- compiler flags utilized in the source code --
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    # -- exceptions and warnings --
    -fexceptions
	-Wcast-align
    ## LDOPT
    ##    -s -Xlinker -rpath -Xlinker
    )
  set (CASA_CXXSTD "${CASA_CXXSTD} -Wreturn-type -Wimplicit -fPIC")
  set (CASA_LDSTD "-Xlinker -rpath -Xlinker")
  IF (APPLE)
    ## definition common to apple architectures
	set (AIPS_DARWIN 1)
    add_definitions (
      -DAIPS_DARWIN
      -DNATIVE_EXCP
      -D_FILE_OFFSET_BITS=64
	  -D_LARGEFILE_SOURCE
      )
    ## PPC specific settings
    if (CMAKE_OSX_ARCHITECTURES MATCHES ppc)
      add_definitions (
	  -DAIPS_BIG_ENDIAN
	)
    elseif (CMAKE_OSX_ARCHITECTURES MATCHES ppc)
      add_definitions (
	  -DAIPS_LITTLE_ENDIAN
	)
    endif (CMAKE_OSX_ARCHITECTURES MATCHES ppc)
  ELSE (APPLE)
    ADD_DEFINITIONS (
      -DAIPS_LINUX
      -DAIPS_LITTLE_ENDIAN
      )
  ENDIF (APPLE)
  ## Platform test 32/64 bit
  set (CMAKE_SYSTEM_64BIT 0)
  if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
      set (CMAKE_SYSTEM_64BIT 1)
	  add_definitions (-DAIPS_64B)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
  endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
ENDIF (UNIX)

SET (CMAKE_MODULE_LINKER_FLAGS ${CASA_LDSTD})
SET (CMAKE_C_FLAGS ${CASA_CXXSTD})

## -----------------------------------------------------------------------------
## Sub-directories

subdirs (${casacore_packages})

## -----------------------------------------------------------------------------
## Installation

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

## -----------------------------------------------------------------------------
## Summary of settings

message (STATUS "casacore_SOURCE_DIR ..... = ${casacore_SOURCE_DIR}")
message (STATUS "CMAKE_SYSTEM ............ = ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_SYSTEM_NAME ....... = ${CMAKE_SYSTEM_NAME}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR .. = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_SYSTEM_64BIT ...... = ${CMAKE_SYSTEM_64BIT}")
message (STATUS "SIZEOF_FLOAT ............ = ${SIZEOF_FLOAT}")
message (STATUS "SIZEOF_DOUBLE ........... = ${SIZEOF_DOUBLE}")
message (STATUS "CMAKE_C_FLAGS ........... = ${CMAKE_C_FLAGS}")
message (STATUS "CMAKE_CXX_FLAGS ......... = ${CMAKE_CXX_FLAGS}")
message (STATUS "CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")
message (STATUS "CMAKE_INSTALL_PREFIX .... = ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Bison lexical parser .... = ${bison_bin}")
message (STATUS "Flex lexical parser ..... = ${flex_bin}")
message (STATUS "libm library ............ = ${libm}")
message (STATUS "CFITSIO header files .... = ${CFITSIO_INCLUDES}")
message (STATUS "CFITSIO library ......... = ${CFITSIO_LIBRARIES}")
message (STATUS "WCSLIB header files ..... = ${WCSLIB_INCLUDES}")
message (STATUS "WCSLIB library .......... = ${WCSLIB_LIBRARIES}")
