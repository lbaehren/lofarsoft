##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## =============================================================================
##
## Main CMake configuration file for building the "casacore" package.
##
## Pending issues:
##  - support building shared libraries
##  - scimath/Mathematics/test/tVanVleck does not link due to missing f2c symbols
##
## =============================================================================

message (STATUS "[CMake package configuration] casacore")

## Name of the project
project (casacore)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## Enforced CMake policy 
cmake_policy (VERSION 2.6)

## include the CTest environment
include (CTest)

## =============================================================================
##
##  User defined build/install options
##
## =============================================================================

option (casacore_FORCE_BUILD       "Force rebuild and local installation" NO  )
option (casacore_BUILD_TESTS       "Build the test programs?"             NO  )
option (casacore_ENABLE_TESTING    "Enable running test programs?"        NO  )
option (casacore_SHARED_LIBRARIES  "Build libraries as shared libraries?" NO  )
option (casacore_WITH_G2C          "Use g2c for compiling Fortran code?"  NO  )
option (casacore_HDF5_SUPPORT      "Enable support for HDF5?"             YES )
option (casacore_VERBOSE_CONFIGURE "Verbose output during configuration?" YES )

## Handle option: testing mode on/off

if (casacore_ENABLE_TESTING)
  ## enable building the tests, otherwise there is nothing to run
  set (casacore_BUILD_TESTS 1)
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

check_include_files (assert.h HAVE_ASSERT_H )
check_include_files (ctype.h  HAVE_CTYPE_H  )
check_include_files (fcntl.h  HAVE_FCNTL_H  )
check_include_files (stdlib.h HAVE_STDLIB_H )
check_include_files (stdio.h  HAVE_STDIO_H  )
check_include_files (string.h HAVE_STRING_H )
check_include_files (unistd.h HAVE_UNISTD_H )

check_type_size ("int"       SIZEOF_INT       )
check_type_size ("float"     SIZEOF_FLOAT     )
check_type_size ("double"    SIZEOF_DOUBLE    )
check_type_size ("long"      SIZEOF_LONG      )
check_type_size ("long long" SIZEOF_LONG_LONG )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${casacore_SOURCE_DIR}
  ${casacore_SOURCE_DIR}/..
  ${casacore_SOURCE_DIR}/../..
  ${casacore_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

set ( CFITSIO_FIND_QUIETLY   YES )
set ( GFORTRAN_FIND_QUIETLY  YES )
set ( G2C_FIND_QUIETLY       YES )
set ( HDF5_FIND_QUIETLY      YES )
set ( LAPACK_FIND_QUIETLY    YES )
set ( WCSLIB_FIND_QUIETLY    YES )

if (USG_ROOT)
  ## load common CMake configuration 
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  ## run CMake scripts to locate required external components
  include (FindCFITSIO)
  include (FindGFortran)
  include (FindLAPACK)
  include (FindWCS)
  include (FindHDF5)
else (USG_ROOT)
  ##
  ## If used standalone, the CMake scripts cannot rely on the additional
  ## infrastructure provided by the code base of the LOFAR User Software
  ## Group; thereby we need to have some local versions of the find 
  ## scripts in place, which handle the location of external components
  ##
  ## [1] Standard search locations for header files
  set (include_locations
    /opt/include
    /opt/local/include
    /sw/include
    /usr/include
    /usr/local/include
    /usr/X11R6/include
    CACHE
    PATH
    "Directories to look for include files"
    FORCE
    )
  ## [2] Standard search locations for libraries
  set (lib_locations
    /opt/lib
    /opt/local/lib
    /sw/lib
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /usr/X11R6/lib
    /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
    CACHE
    PATH
    "Directories to look for libraries"
    FORCE
    )
  ## [3] Load CMake modules
  include (${casacore_SOURCE_DIR}/FindCFITSIO.cmake)
  include (${casacore_SOURCE_DIR}/FindG2C.cmake)
  include (${casacore_SOURCE_DIR}/FindLAPACK.cmake)
  include (${casacore_SOURCE_DIR}/FindWCS.cmake)
  include (${casacore_SOURCE_DIR}/FindHDF5.cmake)
endif (USG_ROOT)

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
## HDF5 library

if (casacore_HDF5_SUPPORT)
  
  if (HAVE_HDF5)
    ## if we have HDF5, is it the correct version?
    if (${HDF5_MAJOR_VERSION} STRLESS 1)
      message (STATUS "Major version of HDF5 too old for casacore!")
      set (HAVE_HDF5 FALSE)
    else (${HDF5_MAJOR_VERSION} STRLESS 1)
      if (${HDF5_MINOR_VERSION} STRLESS 8)
	message (STATUS "HDF5 library version is < 1.8")
	set (HAVE_HDF5 FALSE)
      else (${HDF5_MINOR_VERSION} STRLESS 8)
	message (STATUS "HDF5 library version is >= 1.8")
      endif (${HDF5_MINOR_VERSION} STRLESS 8)
    endif (${HDF5_MAJOR_VERSION} STRLESS 1)
  else (HAVE_HDF5)
    message (STATUS "Unable to locate HDF5; disabling support with casacore.")
    set (HAVE_HDF5 FALSE)
  endif (HAVE_HDF5)

  if  (HAVE_HDF5)
    include_directories (${HDF5_INCLUDES})
    add_definitions (-DHAVE_HDF5)
  else (HAVE_HDF5)
    set (casacore_HDF5_SUPPORT FALSE)
  endif (HAVE_HDF5)
  
endif (casacore_HDF5_SUPPORT)

## -----------------------------------------------------------------------------
## Check for additional tools which are required

find_program (bison_bin bison ${bin_locations} )
find_program (flex_bin flex   ${bin_locations} )
find_program (uname_bin uname ${bin_locations} )

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
    )
  IF (APPLE)
    ## definition common to apple architectures
    add_definitions (
      -DAIPS_DARWIN
      )
  ELSE (APPLE)
    ADD_DEFINITIONS (
      -DAIPS_LINUX
      )
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
## Create configured header file

configure_file (${USG_ROOT}/devel_common/cmake/config_casacore.h.in
  ${casacore_BINARY_DIR}/casacore.h)

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
##  Retrieve the measures data tables providing observatory info, ephimerides,
##  etc.
##
## =============================================================================

include (GetCasacoreData)

find_path (HAVE_EPHEMERIDES table.dat
  PATHS ${USG_ROOT}/data/ephemerides
  PATH_SUFFIXES DE200 DE405 Sources
  )

find_path (HAVE_GEODETIC table.dat
  PATHS ${USG_ROOT}/data/geodetic
  PATH_SUFFIXES Observatories SCHED_locations TAI_UTC
  )

## =============================================================================
##
##  Summary of settings
##
## =============================================================================

if (casacore_VERBOSE_CONFIGURE)
  
  message (STATUS " ")
  message (STATUS "======================================================================")
  message (STATUS " ")
  message (STATUS "[casacore] Configuration summary")
  message (STATUS " ")
  message (STATUS "casacore_SOURCE_DIR ........ = ${casacore_SOURCE_DIR}")
  message (STATUS "casacore_BINARY_DIR ........ = ${casacore_SOURCE_DIR}")
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
  message (STATUS "Measures data directory .... = ${casacore_measures_data}")
  message (STATUS "Curl network downloader .... = ${curl_bin}")
  message (STATUS "Wget network downloader .... = ${WGET_EXECUTABLE}")
  message (STATUS "Bison lexical parser ....... = ${bison_bin}")
  message (STATUS "Flex lexical parser ........ = ${flex_bin}")
  message (STATUS "Have libm? ................. = ${libm}")
  message (STATUS "Have libgfortran? .......... = ${GFORTRAN_LIBRARIES}")
  message (STATUS "Have libg2c? ............... = ${G2C_LIBRARIES}")
  message (STATUS "Have CFITSIO? .............. = ${HAVE_CFITSIO}")
  message (STATUS "Have HDF5? ................. = ${HAVE_HDF5}")
  message (STATUS "HDF5 library version ....... = ${HDF5_MAJOR_VERSION}.${HDF5_MINOR_VERSION}")
  message (STATUS "Have LAPACK? ............... = ${HAVE_LAPACK}")
  message (STATUS "Have WCSLIB? ............... = ${HAVE_WCSLIB}")
  message (STATUS " ")
  message (STATUS "======================================================================")
  message (STATUS " ")

endif (casacore_VERBOSE_CONFIGURE)

## =============================================================================
##
##  Check on build conditions
##
## =============================================================================

if (NOT GFORTRAN_LIBRARIES)
  message (STATUS "[casacore] Missing gfortran library!")
endif (NOT GFORTRAN_LIBRARIES)
