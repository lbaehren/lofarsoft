##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## =============================================================================
##
## Main CMake configuration file for building the "casacore" package.
##
## Pending issues:
##  - scimath/Mathematics/test/tVanVleck does not link due to missing f2c symbols
##
## =============================================================================

message (STATUS "[CMake package configuration] casacore")

## Name of the project
project (casacore)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## User defined build/install options

option (casacore_BUILD_TESTS "Build the test programs?" 0)
option (casacore_ENABLE_TESTING "Enable running test programs?" 0)

## Handle option: testing mode on/off

if (casacore_ENABLE_TESTING)
  ## enable building the tests, otherwise there is nothing to run
  set (casacore_BUILD_TESTS 1)
  ## include the CTest environment
  include (CTest)
  ## enable testing
  enable_testing()
endif (casacore_ENABLE_TESTING)

## -----------------------------------------------------------------------------
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
# images      : lattices components mirlib
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
## Additional required CMake functionality

include (CPack)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckTypeSize)
include (TestBigEndian)

## -----------------------------------------------------------------------------
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

set (bin_locations
  ${casacore_SOURCE_DIR}/../release/bin
  ${casacore_SOURCE_DIR}/../../release/bin
  /usr/bin
  /usr/local/bin
  /sw/bin
  /sw/lib/gcc4.2/bin
  )

set (include_locations
  ${casacore_SOURCE_DIR}/../release/include
  ${casacore_SOURCE_DIR}/../../release/include
  /usr/include
  /usr/local/include
  /sw/include
  /sw/lib/gcc4.2/include
  /opt/casa/local/include
  /opt/gcc-4.2.1/include/c++/4.2.1
)

set (lib_locations
  ${casacore_SOURCE_DIR}/../release/lib
  ${casacore_SOURCE_DIR}/../../release/lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /sw/lib/gcc4.2/lib
  /opt/gcc-4.2.1/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  )

check_include_files (assert.h HAVE_ASSERT_H)
check_include_files (ctype.h HAVE_CTYPE_H)
check_include_files (fcntl.h HAVE_FCNTL_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (stdio.h HAVE_STDIO_H)
check_include_files (string.h HAVE_STRING_H)
check_include_files (unistd.h HAVE_UNISTD_H)

check_type_size ("int" SIZEOF_INT)
check_type_size ("float" SIZEOF_FLOAT)
check_type_size ("double" SIZEOF_DOUBLE)
check_type_size ("long" SIZEOF_LONG)
check_type_size ("long long" SIZEOF_LONG_LONG)

## -----------------------------------------------------------------------------
## Required external libraries

## Locate additional CMake find scripts

find_path (casacore_cmake FindCFITSIO.cmake FindG2C.cmake FindLAPACK.cmake
  PATHS 
  ${casacore_SOURCE_DIR}
  ${casacore_SOURCE_DIR}/..
  ${casacore_SOURCE_DIR}/../..
  ${casacore_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (casacore_cmake)
  message (STATUS "[casacore] Using USG CMake find scripts")
  include (${casacore_cmake}/FindCFITSIO.cmake)
  include (${casacore_cmake}/FindG2C.cmake)
  include (${casacore_cmake}/FindLAPACK.cmake)
  include (${casacore_cmake}/FindWCS.cmake)
elseif (casacore_cmake)
  message (STATUS "[casacore] Using local CMake find scripts")
  include (./FindCFITSIO.cmake)
  include (./FindG2C.cmake)
  include (./FindLAPACK.cmake)
  include (./FindWCS.cmake)
endif (casacore_cmake)

## --- libm -----------------------------------------------

find_library (libm m PATHS ${lib_locations})

## --- WCSLIB ---------------------------------------------
##
## As the source code of WCSLIB is in the USG code repository,
## we do have the possiblity to build the library from there
## in case no system-wide installation can be found; this of
## course only will be possible, if the "external" directory
## has been checked out completely.

if (NOT HAVE_WCSLIB)
  message (SEND_ERROR "WCSLIB_INCLUDES  = ${WCSLIB_INCLUDES}")
  message (SEND_ERROR "WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")
  message (FATAL_ERROR "Unable to find system-wide installation of WCSLIB!")
  ## try to locate the source directory first
  find_path (WCSLIB_INCLUDES wcs.h wcshdr.h 
    PATHS
    ${include_locations}
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

find_program (bison_bin bison ${bin_locations})
find_program (flex_bin flex ${bin_locations})
find_program (uname_bin uname ${bin_locations})

## =============================================================================
##
##  Compiler and Linker options
##
## =============================================================================

## -----------------------------------------------------------------------------
## Location of the header files

include_directories (
  ${casacore_SOURCE_DIR}/casa
  ${casacore_SOURCE_DIR}/tables
  ${casacore_SOURCE_DIR}/mirlib
  ${casacore_SOURCE_DIR}/scimath
  ${casacore_SOURCE_DIR}/measures
  ${casacore_SOURCE_DIR}/fits
  ${casacore_SOURCE_DIR}/coordinates
  ${casacore_SOURCE_DIR}/components
  ${casacore_SOURCE_DIR}/lattices
  ${casacore_SOURCE_DIR}/ms
  ${casacore_SOURCE_DIR}/images
  ${casacore_SOURCE_DIR}/msfits
  ${casacore_SOURCE_DIR}/msvis
  ${casacore_SOURCE_DIR}/tasking
  ${casacore_SOURCE_DIR}/images/images/Images
  ${casacore_SOURCE_DIR}/ms/ms/MeasurementSets
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  ${CFITSIO_INCLUDES}
  ${WCSLIB_INCLUDES}
  ${WCSLIB_INCLUDES}/..
  )

## --- Compiler settings -------------------------

IF (UNIX)
  add_definitions (
    -fPIC
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
  ##
  ## Platform test Big/Little Endian ------------------------------------
  ##
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    message (STATUS "System is big endian.")
    add_definitions (-DAIPS_BIG_ENDIAN)
    set (CMAKE_SYSTEM_BIG_ENDIAN 1)
  else (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    message (STATUS "System is little endian.")
#    add_definitions (-DAIPS_LITTLE_ENDIAN)
    set (CMAKE_SYSTEM_BIG_ENDIAN 0)
  endif (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
#  TEST_BIG_ENDIAN (CMAKE_SYSTEM_BIG_ENDIAN)
#  if (CMAKE_SYSTEM_BIG_ENDIAN)
#    message (STATUS "System is big endian.")
#    add_definitions (-DAIPS_BIG_ENDIAN)
#  elseif (CMAKE_SYSTEM_BIG_ENDIAN)
#    message (STATUS "System is little endian.")
#    add_definitions (-DAIPS_LITTLE_ENDIAN)
#  endif (CMAKE_SYSTEM_BIG_ENDIAN)
  ##
  ## Platform test 32/64 bit ------------------------------
  ##
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
ENDIF (UNIX)

SET (CMAKE_MODULE_LINKER_FLAGS "-Xlinker -rpath -Xlinker")

## -----------------------------------------------------------------------------
## Sub-directories

foreach (casacore_package ${casacore_packages})
  add_subdirectory (${casacore_package})
endforeach (casacore_package)

## =============================================================================
##
##  Installation
##
## =============================================================================

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

## =============================================================================
##
##  Summary of settings
##
## =============================================================================

message (STATUS "casacore_SOURCE_DIR ........ = ${casacore_SOURCE_DIR}")
message (STATUS "CMAKE_SYSTEM ............... = ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR ..... = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_SYSTEM_64BIT ......... = ${CMAKE_SYSTEM_64BIT}")
message (STATUS "CMAKE_SYSTEM_BIG_ENDIAN .... = ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS "SIZEOF_FLOAT ............... = ${SIZEOF_FLOAT}")
message (STATUS "SIZEOF_DOUBLE .............. = ${SIZEOF_DOUBLE}")
message (STATUS "SIZEOF_LONG ................ = ${SIZEOF_LONG}")
message (STATUS "SIZEOF_LONG_LONG ........... = ${SIZEOF_LONG_LONG}")
message (STATUS "CMAKE_CXX_COMPILER ......... = ${CMAKE_CXX_COMPILER}")
message (STATUS "CMAKE_CXX_COMPILER_ID ...... = ${CMAKE_CXX_COMPILER_ID}")
message (STATUS "CMAKE_C_FLAGS .............. = ${CMAKE_C_FLAGS}")
message (STATUS "CMAKE_CXX_FLAGS ............ = ${CMAKE_CXX_FLAGS}")
message (STATUS "CMAKE_MODULE_LINKER_FLAGS .. = ${CMAKE_MODULE_LINKER_FLAGS}")
message (STATUS "CMAKE_INSTALL_PREFIX ....... = ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Bison lexical parser ....... = ${bison_bin}")
message (STATUS "Flex lexical parser ........ = ${flex_bin}")
message (STATUS "libm library ............... = ${libm}")
message (STATUS "CFITSIO header files ....... = ${CFITSIO_INCLUDES}")
message (STATUS "CFITSIO library ............ = ${CFITSIO_LIBRARIES}")
message (STATUS "LAPACK header files ........ = ${LAPACK_INCLUDES}")
message (STATUS "LAPACK library ............. = ${LAPACK_LIBRARIES}")
message (STATUS "WCSLIB header files ........ = ${WCSLIB_INCLUDES}")
message (STATUS "WCSLIB library ............. = ${WCSLIB_LIBRARIES}")
