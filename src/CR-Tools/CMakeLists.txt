
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

## system libraries

find_library (HAVE_LIBM    m     PATHS ${lib_locations} )
find_library (HAVE_LIBUTIL util  PATHS ${lib_locations} )
find_library (HAVE_LIBDL   dl    PATHS ${lib_locations} )

find_library (libgd gd
  PATHS ${lib_locations}
  )

## Standard CMake modules

find_package (Motif)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## Custom CMake modules

include (FindBlitz)
include (FindBoost)
include (FindCFITSIO)
include (FindDAL)
include (FindFFTW3)
include (FindGIF)
include (FindGSL)
include (FindGTK)
include (FindHDF5)
include (FindPNG)
include (FindROOT)
include (FindWCS)

## --- CASA / casacore -------------------------

if (CR_WITH_GLISH)
  ## load find script
  include (${cr_cmake}/FindGlish.cmake)
  ## process outcome 
  if (HAVE_GLISH)
    include_directories (${GLISH_INCLUDES})
  else (HAVE_GLISH)
    message (STATUS "[CR-Tools] Disabling Glish support - missing Glish components.")
    set (CR_WITH_GLISH FALSE)
  endif (HAVE_GLISH)
endif (CR_WITH_GLISH)

## keep in mind, that we only are going to need the
## fully-blown CASA installation if we want the Glish
## support; otherwise we use casacore

if (CR_WITH_GLISH)
  message (STATUS "[CR-Tools] Configuration with Glish support -- CASA")
  ## load find script
  include (${cr_cmake}/FindCASA.cmake)
  ## process outcome
  if (NOT HAVE_CASA)
    set (CR_WITH_GLISH 0)
  endif (NOT HAVE_CASA)
else (CR_WITH_GLISH)
  message (STATUS "[CR-Tools] Configuration without Glish support -- casacore")
  ## load find script
  include (FindCASACORE)
  ## process outcome
  if (HAVE_CASACORE)
    add_definitions (-DHAVE_CASACORE)
  endif (HAVE_CASACORE)
  ## CMake variables
  set (HAVE_CASA FALSE)
  set (HAVE_GLISH FALSE)
endif (CR_WITH_GLISH)

if (APPLE)
  message (STATUS "[CR-Tools] casacore configuration for MAC OS X")
  ## this surely is not the best solution, but for the time being I am 
  ## using the configuration here which I have worked out in order to build
  ## the code outside the USG framework
  if (HAVE_CASA OR HAVE_CASACORE)
    add_definitions (
      -DWITH_CASA
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
else (APPLE)
  ## compiler flags
  add_definitions (
    # -- compiler flags utilized in the source code --
    -DWITH_CASA
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    )
  IF (APPLE)
    ## definition common to apple architectures
    add_definitions (
      -DAIPS_DARWIN
      )
  ELSE (APPLE)
    add_definitions (-DAIPS_LINUX)
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
endif (APPLE)

## --- G2C -------------------------------------

include (${cr_cmake}/FindG2C.cmake)

if (HAVE_G2C)
  add_definitions (-DHAVE_G2C)
  include_directories (${G2C_INCLUDES})
endif (HAVE_G2C)

## LAPACK --------------------------------------

include (${cr_cmake}/FindLAPACK.cmake)

if (LAPACK_INCLUDES)
  add_definitions (-DHAVE_LAPACK)
  include_directories (${LAPACK_INCLUDES})
endif (LAPACK_INCLUDES)

## -- PGPlot -----------------------------------

include (${cr_cmake}/FindPGPLOT.cmake)

if (PGPLOT_INCLUDES)
  include_directories (${PGPLOT_INCLUDES})
  add_definitions (-DHAVE_PGPLOT)
endif (PGPLOT_INCLUDES)

## -- PLplot -----------------------------------

include (${cr_cmake}/FindPLplot.cmake)

if (PLPLOT_INCLUDES)
  include_directories (${PLPLOT_INCLUDES})
  add_definitions (-DHAVE_PLPLOT)
endif (PLPLOT_INCLUDES)

## -- popt -------------------------------------

include (${cr_cmake}/FindPOPT.cmake)

if (HAVE_POPT)
  add_definitions (-DHAVE_POPT)
  include_directories (${POPT_INCLUDES})
endif (HAVE_POPT)

## -- PostgreSQL -------------------------------

include (${cr_cmake}/FindPostgreSQL.cmake)

if (HAVE_PostgreSQL)
  add_definitions (-DHAVE_POSTGRESQL)
  include_directories (${PostgreSQL_INCLUDES})
endif (HAVE_PostgreSQL)

## -- Python -----------------------------------

include (${cr_cmake}/FindPython.cmake)

if (HAVE_PYTHON)
  add_definitions (-DHAVE_PYTHON)
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
  add_definitions (-DDEBUGGING_MESSAGES)
endif (CR_DEBUGGING_MESSAGES)

## -------------------------------------------------------------------
## Handle option: Enable standard set of compiler warnings? ON/OFF

if (CR_COMPILER_WARNINGS)
  set (CMAKE_CXX_FLAGS
    "-Wall -Wno-comment -Woverloaded-virtual -Wno-non-template-friend"
    )
endif (CR_COMPILER_WARNINGS)
    
## -------------------------------------------------------------------
## Handle option: create Python binding

if (NOT HAVE_PYTHON OR NOT HAVE_BOOST)
  set (CR_WITH_PYTHON OFF)
endif (NOT HAVE_PYTHON OR NOT HAVE_BOOST)

## -------------------------------------------------------------------
## Handle option: Activate support for generation of plots ON/OFF

if (CR_WITH_PLOTTING)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    set (CR_WITH_PLOTTING ON)
  else (HAVE_PGPLOT OR HAVE_PLPLOT)
    message (STATUS "[CR-Tools] Disabling support for plotting.")
    set (CR_WITH_PLOTTING OFF)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_PLOTTING)

## Option: Use routines from Star-Tools package? ON/OFF

if (CR_WITH_STARTOOLS)
  
  include (FindStarTools)

  if (HAVE_STARTOOLS)
    include_directories (${STARTOOLS_INCLUDES})
  else (HAVE_STARTOOLS)
    set (CR_WITH_STARTOOLS NO)
  endif (HAVE_STARTOOLS)
  
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

## =============================================================================
##
##  Installation
##
## =============================================================================

find_path (prefix release_area.txt
  PATHS
  ${CR_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

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

if (BOOST_INCLUDES)
  include_directories (${BOOST_INCLUDES})
endif (BOOST_INCLUDES)

if (CASA_INCLUDES)
  add_definitions (-DHAVE_CASA)
  include_directories (${CASA_INCLUDES})
endif (CASA_INCLUDES)

if (CFITSIO_INCLUDES)
  include_directories (${CFITSIO_INCLUDES})
endif (CFITSIO_INCLUDES)

if (DAL_INCLUDES)
  include_directories (${DAL_INCLUDES})
endif (DAL_INCLUDES)

if (FFTW3_INCLUDES)
  add_definitions (-DHAVE_FFTW3)
  include_directories (${FFTW3_INCLUDES})
endif (FFTW3_INCLUDES)

if (GIF_INCLUDES)
  add_definitions (-DHAVE_GIF)
  include_directories (${GIF_INCLUDES})
endif (GIF_INCLUDES)

if (GSL_INCLUDES)
  add_definitions (-DHAVE_GSL)
  include_directories (${GSL_INCLUDES})
endif (GSL_INCLUDES)

if (GTK_INCLUDES)
  add_definitions (-DHAVE_GTK)
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

## Extra tests for using external libraries

if (CR_BUILD_EXTERNAL_TESTS)
  add_subdirectory (test)
endif (CR_BUILD_EXTERNAL_TESTS)

## Sources to build libcr

add_subdirectory (implement)

## Applications

add_subdirectory (apps)

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

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
