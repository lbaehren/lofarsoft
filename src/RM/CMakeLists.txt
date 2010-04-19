
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 2289 2009-02-03 17:33:37Z baehren                       $
##-------------------------------------------------------------------------------

## Name of the project
project (RM)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## Load CMake modules
include (CTest)
enable_testing()

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${RM_SOURCE_DIR}
    ${RM_SOURCE_DIR}/..
    ${RM_SOURCE_DIR}/../..
    ${RM_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )
  
  if (USG_ROOT)
    include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (USG_ROOT)
endif (NOT USG_ROOT)

## =============================================================================
##
##  Options
##
## =============================================================================

option (RM_COMPILER_WARNINGS    "Enable standard set of compiler warnings?"  YES )
option (RM_DEBUGGING_MESSAGES   "Print debugging information?"               NO  )
option (RM_VERBOSE_CONFIGURE    "Verbose output during configuration?"       NO  )
option (RM_ENABLE_ITPP          "Enable using IT++ library?"                 NO  )
option (RM_ENABLE_ARMADILLO     "Enable using Armadillo library?"            YES )

## =============================================================================
##
##  External dependencies
##
## =============================================================================

## Verbose during configure? If not, then mute the output from the search scripts

if (NOT RM_VERBOSE_CONFIGURE)
  ## output of the find scripts
  set (ARMADILLO_FIND_QUIETLY  YES )
  set (BOOST_FIND_QUIETLY      YES )
  set (CASA_FIND_QUIETLY       YES )
  set (CASACORE_FIND_QUIETLY   YES )
  set (CFITSIO_FIND_QUIETLY    YES )
  set (DAL_FIND_QUIETLY        YES )
  set (FFTW3_FIND_QUIETLY      YES )
  set (GFORTRAN_FIND_QUIETLY   YES )
  set (HDF5_FIND_QUIETLY       YES )
  set (ITPP_FIND_QUIETLY       YES )
  set (LAPACK_FIND_QUIETLY     YES )
  set (PGPLOT_FIND_QUIETLY     YES )
  set (PLPLOT_FIND_QUIETLY     YES )
  set (POPT_FIND_QUIETLY       YES )
  set (POSTGRESQL_FIND_QUIETLY YES )
  set (PYTHON_FIND_QUIETLY     YES )
  set (WCSLIB_FIND_QUIETLY     YES )
  set (ZLIB_FIND_QUIETLY       YES )
  ## output of the generated makefile
  set (CMAKE_VERBOSE_MAKEFILE  NO  )
endif (NOT RM_VERBOSE_CONFIGURE)

## Check functions -------------------------------

include (CheckFunctionExists)

check_function_exists (abs   HAVE_ABS )
check_function_exists (fabs  HAVE_FABS )

## Standard CMake modules ------------------------

find_package (Motif)
find_package (X11)
find_package (ZLIB)

## Custom CMake modules --------------------------

include (FindArmadillo)
include (FindBoost)
include (FindCASACORE)
include (FindCFITSIO)
include (FindDAL)
include (FindFFTW3)
include (FindGFortran)
include (FindHDF5)
include (FindLAPACK)
include (FindPython)
include (FindWCSLIB)

if (RM_ENABLE_ITPP)
  include (FindITPP)
endif (RM_ENABLE_ITPP)

## =============================================================================
##
##  Handling of configuration/build/install options
##
## =============================================================================

## -------------------------------------------------------------------
## Handle option: debugging messages on/off

if (RM_DEBUGGING_MESSAGES)
  set (DEBUGGING_MESSAGES ON)
endif (RM_DEBUGGING_MESSAGES)

## -------------------------------------------------------------------
## Handle option: Enable standard set of compiler warnings?  ON/OFF

if (RM_COMPILER_WARNINGS)
  set (CMAKE_CXX_FLAGS
    "-Wall -g -Wno-comment -Woverloaded-virtual -Wno-non-template-friend"
    )
endif (RM_COMPILER_WARNINGS)
    
## Handle configuration to use CASA/casacore

if (HAVE_CASA OR HAVE_CASACORE)
  
  ## compiler flags
  set (HAVE_CASA YES)

  ## Platform test 32/64 bit ------------------------------
  set (CMAKE_SYSTEM_64BIT 0)
  if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
      if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
        set (CMAKE_SYSTEM_64BIT 1)
        set (AIPS_64B YES)
      endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
  endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
  if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
      set (CMAKE_SYSTEM_64BIT YES)
      set (AIPS_64B YES)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
  endif (APPLE)
  
endif (HAVE_CASA OR HAVE_CASACORE)

## =============================================================================
##
##  Installation
##
## =============================================================================

## No relinking and full RPATH for the install tree
## (http://www.cmake.org/Wiki/CMake_RPATH_handling)

# skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH TRUE)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

## =============================================================================
##
##  Location of (external) header files
##
## =============================================================================

## RM-Tools header files

include_directories (BEFORE
  ${RM_BINARY_DIR}
  ${RM_SOURCE_DIR}
  ${RM_SOURCE_DIR}/implement
  ${RM_SOURCE_DIR}/implement/rmLib
  ${RM_SOURCE_DIR}/implement/rmClean
  ${RM_SOURCE_DIR}/implement/rmSim
  ${RM_SOURCE_DIR}/implement/rmSynthesis
  ${RM_SOURCE_DIR}/implement/rmWiener
  ${RM_SOURCE_DIR}/apps/rmcmd
  ${RM_SOURCE_DIR}/test
  )

## External header files

if (HAVE_ARMADILLO)
  include_directories (${ARMADILLO_INCLUDES})
  add_definitions (-DHAVE_ARMADILLO)
else (HAVE_ARMADILLO)
  message (STATUS "[RM] Armadillo installation incomplete!")
endif (HAVE_ARMADILLO)

if (HAVE_DAL)
  include_directories (${DAL_INCLUDES})
  ## additional compiler flags
  if (HAVE_CASA OR HAVE_CASACORE)
    set (WITH_CASA YES)
    add_definitions (-DWITH_CASA)
  endif (HAVE_CASA OR HAVE_CASACORE)
endif (HAVE_DAL)

if (HAVE_CASACORE)
  include_directories (${CASACORE_INCLUDES})
  add_definitions (-DHAVE_CASA)
else (HAVE_CASACORE)
  message (STATUS "[RM] Missing or incomplete installation of casacore!")
endif (HAVE_CASACORE)

if (HAVE_CFITSIO)
  include_directories (${CFITSIO_INCLUDES})
  add_definitions (-DHAVE_CFITSIO)
else (HAVE_CFITSIO)
  message (STATUS "[RM] Missing or incomplete installation of CFITSIO!")
endif (HAVE_CFITSIO)

if (HAVE_FFTW3)
  include_directories (${FFTW3_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
else (HAVE_FFTW3)
  message (STATUS "[RM] Missing or incomplete installation of FFTW3!")
endif (HAVE_FFTW3)

if (HAVE_HDF5)
  include_directories (${HDF5_INCLUDES})
  add_definitions (-DHAVE_HDF5)
else (HAVE_HDF5)
  message (STATUS "[RM] HDF5 installation incomplete!")
endif (HAVE_HDF5)

if (RM_ENABLE_ITPP AND HAVE_ITPP)
  include_directories (${ITPP_INCLUDES})
  add_definitions (-DHAVE_ITPP)
else (RM_ENABLE_ITPP AND HAVE_ITPP)
  message (STATUS "[RM] IT++ installation incomplete!")
endif (RM_ENABLE_ITPP AND HAVE_ITPP)

if (HAVE_WCSLIB)
  include_directories (${WCSLIB_INCLUDES})
  add_definitions (-DHAVE_WCSLIB)
else (HAVE_WCSLIB)
  message (STATUS "[RM] Missing or incomplete installation of WCSLIB!")
endif (HAVE_WCSLIB)

## =============================================================================
##
##  External libraries
##
## =============================================================================

set (rm_link_libraries
  ${DAL_LIBRARIES}
  ${CASA_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${FFTW3_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )

if (ARMADILLO_LIBRARIES)
  list (APPEND rm_link_libraries ${ARMADILLO_LIBRARIES})
endif (ARMADILLO_LIBRARIES)

if (ITPP_LIBRARIES)
  list (APPEND rm_link_libraries ${ITPP_LIBRARIES})
endif (ITPP_LIBRARIES)

if (HDF5_LIBRARIES)
  list (APPEND rm_link_libraries ${HDF5_LIBRARIES})
endif (HDF5_LIBRARIES)

if (HAVE_LIBZ)
  list (APPEND rm_link_libraries ${HAVE_LIBZ})
endif (HAVE_LIBZ)

if (HAVE_LIBDL)
  list (APPEND rm_link_libraries ${HAVE_LIBDL})
endif (HAVE_LIBDL)

## =============================================================================
##
##  Configuration for the subdirectories
##
## =============================================================================

add_subdirectory (implement)
add_subdirectory (test)
add_subdirectory (apps)

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

message (STATUS "+============================================================+")
message (STATUS "| RM-Tools: Summary of configuration settings                |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " CMAKE_SYSTEM ............. : ${CMAKE_SYSTEM}")
message (STATUS " CMAKE_SYSTEM_VERSION ..... : ${CMAKE_SYSTEM_VERSION}")
message (STATUS " CMAKE_SYSTEM_PROCESSOR ... : ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " CMAKE_SYSTEM_64BIT ........: ${CMAKE_SYSTEM_64BIT}")
message (STATUS " CMAKE_SYSTEM_BIG_ENDIAN .. : ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS " CMAKE_MODULE_PATH ........ : ${CMAKE_MODULE_PATH}")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " CFITSIO_INCLUDES ......... : ${CFITSIO_INCLUDES}")
message (STATUS " CFITSIO_LIBRARIES ........ : ${CFITSIO_LIBRARIES}")
message (STATUS " DAL_INCLUDES ............. : ${DAL_INCLUDES}")
message (STATUS " DAL_LIBRARIES ............ : ${DAL_LIBRARIES}")
message (STATUS " HDF5_INCLUDES ............ : ${HDF5_INCLUDES}")
message (STATUS " HDF5_LIBRARIES ........... : ${HDF5_LIBRARIES}")
message (STATUS " ITPP_INCLUDES ............ : ${ITPP_INCLUDES}")
message (STATUS " ITPP_LIBRARIES ........... : ${ITPP_LIBRARIES}")
message (STATUS "+============================================================+")
