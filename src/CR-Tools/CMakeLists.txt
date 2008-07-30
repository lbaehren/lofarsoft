
##-------------------------------------------------------------------------------
## $Id::                                                                        $
##-------------------------------------------------------------------------------

## Name of the project
project (CR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${CR_SOURCE_DIR}
    ${CR_SOURCE_DIR}/..
    ${CR_SOURCE_DIR}/../..
    ${CR_SOURCE_DIR}/../../..
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

option (CR_BUILD_TESTS        "Build the test programs?"                     YES )
option (CR_BUILD_EXTERNAL_TESTS "Build test programs for external packages?" YES )
option (CR_ENABLE_TESTING     "Enable running the test programs?"            YES )
option (CR_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"    YES )
option (CR_DEBUGGING_MESSAGES "Print debugging information?"                 NO  )
option (CR_VERBOSE_CONFIGURE  "Verbose output during configuration?"         NO  )
option (CR_WITH_GLISH         "Support for Glish applications?"              NO  )
option (CR_WITH_PYTHON        "Support for Python applications?"             YES )
option (CR_WITH_PLOTTING      "Support for generation of plots?"             YES )
option (CR_WITH_GUI           "Build the graphical interface?"               YES )
option (CR_WITH_STARTOOLS     "Enable additional routines from LOPES-Star?"  YES )

## =============================================================================
##
##  External dependencies
##
## =============================================================================

## Verbose during configure? If not, then mute the output from the search scripts

if (NOT CR_VERBOSE_CONFIGURE)
  ## output of the find scripts
  set (BLITZ_FIND_QUIETLY      YES )
  set (BOOST_FIND_QUIETLY      YES )
  set (CASA_FIND_QUIETLY       YES )
  set (CASACORE_FIND_QUIETLY   YES )
  set (CFITSIO_FIND_QUIETLY    YES )
  set (DAL_FIND_QUIETLY        YES )
  set (FFTW3_FIND_QUIETLY      YES )
  set (G2C_FIND_QUIETLY        YES )
  set (GIF_FIND_QUIETLY        YES )
  set (GLISH_FIND_QUIETLY      YES )
  set (GSL_FIND_QUIETLY        YES )
  set (GTK_FIND_QUIETLY        YES )
  set (HDF5_FIND_QUIETLY       YES )
  set (LAPACK_FIND_QUIETLY     YES )
  set (PGPLOT_FIND_QUIETLY     YES )
  set (PLPLOT_FIND_QUIETLY     YES )
  set (PNG_FIND_QUIETLY        YES )
  set (POPT_FIND_QUIETLY       YES )
  set (PostgreSQL_FIND_QUIETLY YES )
  set (PYTHON_FIND_QUIETLY     YES )
  set (ROOT_FIND_QUIETLY       YES )
  set (STARTOOLS_FIND_QUIETLY  YES )
  set (TCL_FIND_QUIETLY        YES )
  set (WCSLIB_FIND_QUIETLY     YES )
  set (ZLIB_FIND_QUIETLY       YES )
  ## output of the generated makefile
  set (CMAKE_VERBOSE_MAKEFILE  NO  )
endif (NOT CR_VERBOSE_CONFIGURE)

## System libraries ------------------------------

find_library (HAVE_LIBM    m     PATHS ${lib_locations} )
find_library (HAVE_LIBUTIL util  PATHS ${lib_locations} )
find_library (HAVE_LIBDL   dl    PATHS ${lib_locations} )
find_library (libgd        gd    PATHS ${lib_locations} )

## System header files ---------------------------

find_path (HAVE_LIBGEN_H     libgen.h      PATHS ${include_locations} )
find_path (HAVE_STDINT_H     stdint.h      PATHS ${include_locations} )
find_path (HAVE_STDLIB_H     stdlib.h      PATHS ${include_locations} )
find_path (HAVE_STRING_H     string.h      PATHS ${include_locations} )
find_path (HAVE_STRINGS_H    strings.h     PATHS ${include_locations} )
find_path (HAVE_SYS_STAT_H   sys/stat.h    PATHS ${include_locations} )
find_path (HAVE_SYS_TIME_H   sys/time.h    PATHS ${include_locations} )
find_path (HAVE_SYS_TYPES_H  sys/types.h   PATHS ${include_locations} )
find_path (HAVE_SYS_UTIME_H  sys/utime.h   PATHS ${include_locations} )

## Check type sizes ------------------------------

include (CheckTypeSize)

check_type_size ("short"         SIZEOF_SHORT       )
check_type_size ("int"           SIZEOF_INT         )
check_type_size ("float"         SIZEOF_FLOAT       )
check_type_size ("double"        SIZEOF_DOUBLE      )
check_type_size ("long"          SIZEOF_LONG        )
check_type_size ("long long"     SIZEOF_LONG_LONG   )

## Standard CMake modules ------------------------

find_package (Motif)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## Custom CMake modules --------------------------

include (FindBlitz)
include (FindBoost)
include (FindCFITSIO)
include (FindDAL)
include (FindFFTW3)
include (FindG2C)
include (FindGIF)
include (FindGSL)
include (FindGTK)
include (FindHDF5)
include (FindLAPACK)
include (FindPGPLOT)
include (FindPLplot)
include (FindPNG)
include (FindPOPT)
include (FindPostgreSQL)
include (FindPython)
include (FindROOT)
include (FindWCS)

if (BOOST_INCLUDES)
  include_directories (${BOOST_INCLUDES})
endif (BOOST_INCLUDES)

if (HAVE_G2C)
  include_directories (${G2C_INCLUDES})
endif (HAVE_G2C)

if (LAPACK_INCLUDES)
  include_directories (${LAPACK_INCLUDES})
endif (LAPACK_INCLUDES)

if (PGPLOT_INCLUDES)
  include_directories (${PGPLOT_INCLUDES})
endif (PGPLOT_INCLUDES)

if (PLPLOT_INCLUDES)
  include_directories (${PLPLOT_INCLUDES})
endif (PLPLOT_INCLUDES)

if (HAVE_POPT)
  include_directories (${POPT_INCLUDES})
endif (HAVE_POPT)

if (HAVE_PostgreSQL)
  set (HAVE_POSTGRESQL 1)
  include_directories (${PostgreSQL_INCLUDES})
endif (HAVE_PostgreSQL)

if (HAVE_PYTHON)
  include_directories (${PYTHON_INCLUDES})
endif (HAVE_PYTHON)


## =============================================================================
##
##  Handling of configuration/build/install options
##
## =============================================================================

## -------------------------------------------------------------------
## Handle option: testing mode on/off

if (CR_ENABLE_TESTING)
  # CMake/CTest settings
  include (CTest)
  enable_testing()
  # enable building test programs
  set (CR_BUILD_TESTS ON)
  set (CR_BUILD_EXTERNAL_TESTS ON)
endif (CR_ENABLE_TESTING)

## -------------------------------------------------------------------
## Handle option: debugging messages on/off

if (CR_DEBUGGING_MESSAGES)
  set (DEBUGGING_MESSAGES ON)
endif (CR_DEBUGGING_MESSAGES)

## -------------------------------------------------------------------
## Handle option: Enable standard set of compiler warnings?  ON/OFF

if (CR_COMPILER_WARNINGS)
  set (CMAKE_CXX_FLAGS
    "-Wall -Wno-comment -Woverloaded-virtual -Wno-non-template-friend"
    )
endif (CR_COMPILER_WARNINGS)
    
## -------------------------------------------------------------------
## Handle option: Support for Glish applications?  ON/OFF

if (CR_WITH_GLISH)

  ## First check if we actually have a working version of Glish; if
  ## this is not the case, we can immediately switch off this option.
  
  include (FindGlish)
  
  if (HAVE_GLISH)
    include_directories (${GLISH_INCLUDES})
  else (HAVE_GLISH)
    set (CR_WITH_GLISH FALSE)
  endif (HAVE_GLISH)
  
  ## If there is a working version of Glish, then in order to use it
  ## we need a full installation of CASA
  
  if (HAVE_GLISH)
    ## Check for installation of CASA
    include (FindCASA)
    ## ... and process the outcome
    if (NOT HAVE_CASA)
      set (CR_WITH_GLISH NO)
    endif (NOT HAVE_CASA)
  endif (HAVE_GLISH)
  
else (CR_WITH_GLISH)
  include (FindCASACORE)
endif (CR_WITH_GLISH)

## Handle configuration to use CASA/casacore

if (HAVE_CASA OR HAVE_CASACORE)
  
  ## compiler flags
  set (HAVE_CASA 1)
  set (WITH_CASA 1)
  add_definitions (
    # -- compiler flags utilized in the source code --
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    )
  IF (APPLE)
    ## definition common to apple architectures
    set (AIPS_DARWIN 1)
  ELSE (APPLE)
    set (AIPS_LINUX 1)
  ENDIF (APPLE)
  ## Platform test 32/64 bit ------------------------------
  set (CMAKE_SYSTEM_64BIT 0)
  if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
      if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
        set (CMAKE_SYSTEM_64BIT 1)
        set (AIPS_64B 1)
      endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
  endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
  if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
      set (CMAKE_SYSTEM_64BIT 1)
      set (AIPS_64B 1)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
  endif (APPLE)
  
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## Handle option: Generate Python bindings for the Library? ON/OFF

if (CR_WITH_PYTHON)

  ## Required external package: Python
  if (NOT HAVE_PYTHON)
    message (STATUS "[CR-Tools] Generation of Python bindings disabled; missing Python!")
    set (CR_WITH_PYTHON OFF)
  endif (NOT HAVE_PYTHON)
  ## Required external package: Boost++
  if (NOT HAVE_BOOST)
    message (STATUS "[CR-Tools] Generation of Python bindings disabled; missing Boost!")
    set (CR_WITH_PYTHON OFF)
  endif (NOT HAVE_BOOST)
  ## Required external package: NumUtil
  if (NOT HAVE_NUM_UTIL)
    message (STATUS "[CR-Tools] Generation of Python bindings disabled; missing NumUtil!")
    set (CR_WITH_PYTHON OFF)
  endif (NOT HAVE_NUM_UTIL)

endif (CR_WITH_PYTHON)

## -------------------------------------------------------------------
## Handle option: Activate support for generation of plots ON/OFF

if (CR_WITH_PLOTTING)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    set (CR_WITH_PLOTTING YES)
    set (NEED_FORTRAN_UNDERSCORES YES)
  else (HAVE_PGPLOT OR HAVE_PLPLOT)
    message (STATUS "[CR-Tools] Disabling support for plotting.")
    set (CR_WITH_PLOTTING NO)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_PLOTTING)

## -------------------------------------------------------------------
## Handle option: Additional routines from LOPES-Star

if (CR_WITH_STARTOOLS)
  
  include (FindStarTools)
  
  if (HAVE_STARTOOLS)
    include_directories (${STARTOOLS_INCLUDES})
  else (HAVE_STARTOOLS)
    set (CR_WITH_STARTOOLS FALSE)
  endif (HAVE_STARTOOLS)
  
  ## check if ROOT is available

  if (NOT HAVE_ROOT)
    message (STATUS "LopesStar support disabled; missing ROOT.")
    set (CR_WITH_STARTOOLS FALSE)
  endif (NOT HAVE_ROOT)

  ## check if FFTW3 is available

  if (NOT HAVE_FFTW3)
    message (STATUS "LopesStar support disabled; missing FFTW3.")
    set (CR_WITH_STARTOOLS FALSE)
  endif (NOT HAVE_FFTW3)

endif (CR_WITH_STARTOOLS)

## -------------------------------------------------------------------
## Option: Create graphical user interface? ON/OFF

if (CR_WITH_GUI)

  ## locate the Qt4 libraries

  find_library (QT4_LIBARY_QTCORE QtCore
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-x11 qt4-x11/lib
    )
  
  find_library (QT4_LIBARY_QTGUI QtGui
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-x11 qt4-x11/lib
    )

  find_path (QT4_INCLUDES Qt/qatomic.h
    PATHS ${include_locations}
    PATH_SUFFIXES lib/qt4-x11/include lib/qt4-x11
    )

  ## process conditions for building the GUI

  if (NOT HAVE_BOOST)
    message (STATUS "[CR-Tools] Unable to build GUI - missing Boost!")
    set (CR_WITH_GUI NO)
  endif (NOT HAVE_BOOST)

  if (NOT QT4_INCLUDES)
    message (STATUS "[CR-Tools] Unable to build GUI - missing Qt4 headers!")
    set (CR_WITH_GUI NO)
  endif (NOT QT4_INCLUDES)

  if (NOT QT4_LIBARY_QTCORE OR NOT QT4_LIBARY_QTGUI)
    message (STATUS "[CR-Tools] Unable to build GUI - Qt4 libraries incomplete!")
    set (CR_WITH_GUI NO)
  endif (NOT QT4_LIBARY_QTCORE OR NOT QT4_LIBARY_QTGUI)

endif (CR_WITH_GUI)

## -------------------------------------------------------------------
## Build test programs for external packages?  ON/OFF

if (CR_BUILD_EXTERNAL_TESTS)
  add_subdirectory (test)
endif (CR_BUILD_EXTERNAL_TESTS)

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
##  Datasets for the test programs
##
## =============================================================================

find_file (dataset_tbb tbb_test_file_v1_0.h5
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  )

find_file (dataset_event 2007.01.31.23:59:33.960.event
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

## =============================================================================
##
##  Location of (external) header files
##
## =============================================================================

if (CASA_INCLUDES)
  include_directories (${CASA_INCLUDES})
endif (CASA_INCLUDES)

if (CFITSIO_INCLUDES)
  include_directories (${CFITSIO_INCLUDES})
endif (CFITSIO_INCLUDES)

if (DAL_INCLUDES)
  include_directories (${DAL_INCLUDES})
endif (DAL_INCLUDES)

if (FFTW3_INCLUDES)
  include_directories (${FFTW3_INCLUDES})
endif (FFTW3_INCLUDES)

if (GIF_INCLUDES)
  include_directories (${GIF_INCLUDES})
endif (GIF_INCLUDES)

if (GSL_INCLUDES)
  include_directories (${GSL_INCLUDES})
endif (GSL_INCLUDES)

if (GTK_INCLUDES)
  include_directories (${GTK_INCLUDES})
endif (GTK_INCLUDES)

if (HDF5_INCLUDES)
  include_directories (${HDF5_INCLUDES})
endif (HDF5_INCLUDES)

if (PNG_INCLUDES)
  include_directories (${PNG_INCLUDES})
endif (PNG_INCLUDES)

if (ROOT_INCLUDES)
  include_directories (${ROOT_INCLUDES})
endif (ROOT_INCLUDES)

if (WCS_INCLUDES)
  include_directories (${WCS_INCLUDES})
endif (WCS_INCLUDES)
  
## =============================================================================
##
##  Configuration for the subdirectories
##
## =============================================================================

## Create "crtools.h"

configure_file (${USG_ROOT}/devel_common/cmake/config_crtools.h.in
  ${CR_BINARY_DIR}/crtools.h)

## Include directories

include_directories (
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/test
  ${CR_BINARY_DIR}
  )

## Sub-directories

add_subdirectory (implement)
add_subdirectory (apps)

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

if (CR_VERBOSE_CONFIGURE)
  message (STATUS "+============================================================+")
  message (STATUS "| CR-Tools: Summary of configuration settings                |")
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS " CMAKE_SYSTEM ............. : ${CMAKE_SYSTEM}")
  message (STATUS " CMAKE_SYSTEM_VERSION ..... : ${CMAKE_SYSTEM_VERSION}")
  message (STATUS " CMAKE_SYSTEM_PROCESSOR ... : ${CMAKE_SYSTEM_PROCESSOR}")
  message (STATUS " CMAKE_SYSTEM_64BIT ........: ${CMAKE_SYSTEM_64BIT}")
  message (STATUS " CMAKE_SYSTEM_BIG_ENDIAN .. : ${CMAKE_SYSTEM_BIG_ENDIAN}")
  message (STATUS " CMAKE_MODULE_PATH ........ : ${CMAKE_MODULE_PATH}")
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS " BLITZ .................... : ${HAVE_BLITZ}")
  message (STATUS " BOOST .................... : ${HAVE_BOOST}")
  message (STATUS " CASA ..................... : ${HAVE_CASA}")
  message (STATUS " CASACORE ................. : ${HAVE_CASACORE}")
  message (STATUS " CFITSIO .................. : ${HAVE_CFITSIO}")
  message (STATUS " Data Access Library (DAL)  : ${HAVE_DAL}")
  message (STATUS " FFTW3 .................... : ${HAVE_FFTW3}")
  message (STATUS " G2C ...................... : ${HAVE_G2C}")
  message (STATUS " GIF ...................... : ${HAVE_GIF}")
  message (STATUS " Glish .................... : ${HAVE_GLISH}")
  message (STATUS " GSL ...................... : ${HAVE_GSL}")
  message (STATUS " GTK ...................... : ${HAVE_GTK}")
  message (STATUS " HDF5 ..................... : ${HAVE_HDF5}")
  message (STATUS " LAPACK ................... : ${HAVE_LAPACK}")
  message (STATUS " PGPLOT ................... : ${HAVE_PGPLOT}")
  message (STATUS " PLplot ................... : ${HAVE_PLPLOT}")
  message (STATUS " PNG ...................... : ${HAVE_PNG}")
  message (STATUS " POPT ..................... : ${HAVE_POPT}")
  message (STATUS " PostgreSQL ............... : ${HAVE_PostgreSQL}")
  message (STATUS " Python ................... : ${HAVE_PYTHON}")
  message (STATUS " ROOT ..................... : ${HAVE_ROOT}")
  message (STATUS " Star-Tools ............... : ${HAVE_STARTOOLS}")
  message (STATUS " WCSLIB ................... : ${HAVE_WCSLIB}")
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS " USG root directory ........ : ${USG_ROOT}")
  message (STATUS " Source directory .......... : ${CR_SOURCE_DIR}")
  message (STATUS " Installation location ..... : ${CMAKE_INSTALL_PREFIX}")
  message (STATUS " Build the test programs ... : ${CR_BUILD_TESTS}")
  message (STATUS " Enable testing ............ : ${CR_ENABLE_TESTING}")
  message (STATUS " Enable LOPES-Star routines  : ${CR_WITH_STARTOOLS}")
  message (STATUS " Support for plotting ...... : ${CR_WITH_PLOTTING}")
  message (STATUS " Support for Glish ......... : ${CR_WITH_GLISH}")
  message (STATUS " Support for Python ........ : ${CR_WITH_PYTHON}")
  message (STATUS " Graphical user interface .. : ${CR_WITH_GUI}")
  message (STATUS " Print debugging messages .. : ${CR_DEBUGGING_MESSAGES}")
  message (STATUS " Verbose configuration ..... : ${CR_VERBOSE_CONFIGURE}")
  message (STATUS "+============================================================+")
endif (CR_VERBOSE_CONFIGURE)
