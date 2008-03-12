
##-------------------------------------------------------------------------------
## $Id::                                                                        $
##-------------------------------------------------------------------------------

## Name of the project
project (CR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## =============================================================================
##
##  User defined build/install options
##
## =============================================================================

option (CR_BUILD_TESTS "Build the test programs?" 1)
option (CR_BUILD_EXTERNAL_TESTS "Build the test programs for external packages?" 0)
option (CR_ENABLE_TESTING "Enable running the test programs?" 1)
option (CR_DEBUGGING_MESSAGES "Print debugging information?" 0)
option (CR_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)
option (CR_GLISH_SUPPORT "Activate support for Glish applications?" 0)
option (CR_PYTHON_SUPPORT "Activate support for Python applications?" 0)

## Handle option: verbose mode on/off

if (NOT CR_VERBOSE_CONFIGURE)
  ## output of the find scripts
  set (BLITZ_FIND_QUIETLY 1)
  set (CASA_FIND_QUIETLY 1)
  set (CASACORE_FIND_QUIETLY 1)
  set (CFITSIO_FIND_QUIETLY 1)
  set (DAL_FIND_QUIETLY 1)
  set (FFTW3_FIND_QUIETLY 1)
  set (G2C_FIND_QUIETLY 1)
  set (GIF_FIND_QUIETLY 1)
  set (GLISH_FIND_QUIETLY 1)
  set (GSL_FIND_QUIETLY 1)
  set (GTK_FIND_QUIETLY 1)
  set (HDF5_FIND_QUIETLY 1)
  set (LAPACK_FIND_QUIETLY 1)
  set (PGPLOT_FIND_QUIETLY 1)
  set (PLPLOT_FIND_QUIETLY 1)
  set (PNG_FIND_QUIETLY 1)
  set (POPT_FIND_QUIETLY 1)
  set (PostgreSQL_FIND_QUIETLY 1)
  set (PYTHON_FIND_QUIETLY 1)
  set (ROOT_FIND_QUIETLY 1)
  set (TCL_FIND_QUIETLY 1)
  set (TK_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
  ## output of the generated makefile
  set (CMAKE_VERBOSE_MAKEFILE 0)
endif (NOT CR_VERBOSE_CONFIGURE)

## -----------------------------------------------------------------------------
## Locate additional CMake find scripts

find_path (cr_cmake FindBlitz.cmake FindCASA.cmake FindCFITSIO.cmake
  PATHS 
  ${CR_SOURCE_DIR}
  ${CR_SOURCE_DIR}/..
  ${CR_SOURCE_DIR}/../..
  ${CR_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## If we are unable to locate the CMake find scripts, we need to abort here;
## otherwise we do not know from where to include the files further down.

if (NOT cr_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (NOT cr_cmake)

## =============================================================================
##
##  External dependencies
##
## =============================================================================

set (lib_locations
  ## local
  ../../release/lib
  ## system-wide
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt/lib
  )

## system libraries

find_library (libm m
  PATHS ${lib_locations}
  )

find_library (libutil util
  PATHS ${lib_locations}
  )

find_library (libgd gd
  PATHS ${lib_locations}
  )

## Standard CMake modules for location of packages

find_package (Motif)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

## Search using custom CMake find scripts

if (cr_cmake)
  
  ## --- Blitz++ ---------------------------------
  
  include (${cr_cmake}/FindBlitz.cmake)
  
  if (HAVE_BLITZ)
    add_definitions (-DHAVE_BLITZ)
    include_directories (${BLITZ_INCLUDES})
  endif (HAVE_BLITZ)
  
  ## --- BOOST -----------------------------------
  
  include (${cr_cmake}/FindBoost.cmake)
  
  if (HAVE_BOOST)
    add_definitions (-DHAVE_BOOST)
    include_directories (${BOOST_INCLUDES})
  endif (HAVE_BOOST)
  
  ## --- CFITSIO ---------------------------------
  
  include (${cr_cmake}/FindCFITSIO.cmake)
  
  if (HAVE_CFITSIO)
    add_definitions (-DHAVE_CFITSIO)
    include_directories (${CFITSIO_INCLUDES})
  endif (HAVE_CFITSIO)
  
  ## --- CASA / casacore -------------------------
  
  include (${cr_cmake}/FindCASACORE.cmake)
  
  if (HAVE_CASACORE)
    add_definitions (-DHAVE_CASACORE)
    add_definitions (-DHAVE_CASA)
    include_directories (${CASA_INCLUDES})
  endif (HAVE_CASACORE)
  
  if (APPLE)
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
  endif (APPLE)
  
  ## --- Data Access Library (DAL) ---------------
  
  include (${cr_cmake}/FindDAL.cmake)
  
  if (HAVE_DAL)
    add_definitions (-DHAVE_DAL)
    include_directories (${DAL_INCLUDES})
  endif (HAVE_DAL)
  
  ## --- FFTW3 -----------------------------------
  
  include (${cr_cmake}/FindFFTW3.cmake)
  
  if (HAVE_FFTW3)
    add_definitions (-DHAVE_FFTW3)
    include_directories (${FFTW3_INCLUDES})
  endif (HAVE_FFTW3)
  
  ## --- G2C -------------------------------------
  
  include (${cr_cmake}/FindG2C.cmake)
  
  if (HAVE_G2C)
    add_definitions (-DHAVE_G2C)
    include_directories (${G2C_INCLUDES})
  endif (HAVE_G2C)
  
  ## --- GIF ---------------------------------------
  
  include (${cr_cmake}/FindGIF.cmake)
  
  if (HAVE_GIF)
    add_definitions (-DHAVE_GIF)
    include_directories (${GIF_INCLUDES})
  endif (HAVE_GIF)
  
  ## --- Glish -------------------------------------
  
  include (${cr_cmake}/FindGlish.cmake)
  
  if (HAVE_GLISH)
    add_definitions (-DHAVE_GLISH)
    include_directories (${GLISH_INCLUDES})
  endif (HAVE_GLISH)
  
  ## --- GNU Scientific Library (GSL) --------------
  
  include (${cr_cmake}/FindGSL.cmake)
  
  if (HAVE_GSL)
    add_definitions (-DHAVE_GSL)
    include_directories (${GSL_INCLUDES})
  endif (HAVE_GSL)
  
  ## --- GTK ---------------------------------------
  
  include (${cr_cmake}/FindGTK.cmake)
  
  if (HAVE_GTK)
    add_definitions (-DHAVE_GTK)
    include_directories (${GTK_INCLUDES})
  endif (HAVE_GTK)
  
  ## --- HDF5 ------------------------------------
  
  include (${cr_cmake}/FindHDF5.cmake)
  
  if (HAVE_HDF5)
    add_definitions (-DHAVE_HDF5)
    include_directories (${HDF5_INCLUDES})
  endif (HAVE_HDF5)
  
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
  
  ## -- PNG --------------------------------------
  
  include (${cr_cmake}/FindPNG.cmake)
  
  if (HAVE_PNG)
    add_definitions (-DHAVE_PNG)
    include_directories (${PNG_INCLUDES})
  endif (HAVE_PNG)
  
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
  
  ## -- ROOT -------------------------------------
  
  include (${cr_cmake}/FindROOT.cmake)
  
  if (HAVE_ROOT)
    add_definitions (-DHAVE_ROOT)
    include_directories (${ROOT_INCLUDES})
  endif (HAVE_ROOT)
  
  ## --- Tcl -------------------------------------
  
  include (${cr_cmake}/FindTCL.cmake)
  
  if (HAVE_TCL)
    add_definitions (-DHAVE_TCL)
    include_directories (${TCL_INCLUDES})
  endif (HAVE_TCL)
  
  ## --- Tk --------------------------------------
  
  include (${cr_cmake}/FindTK.cmake)
  
  if (HAVE_TK)
    add_definitions (-DHAVE_TK)
    include_directories (${TK_INCLUDES})
  endif (HAVE_TK)
  
  ## -- WCSLIB -----------------------------------
  
  include (${cr_cmake}/FindWCS.cmake)
  
  if (HAVE_WCS)
    add_definitions (-DHAVE_WCS)
    include_directories (${WCS_INCLUDES})
  endif (HAVE_WCS)
  
endif (cr_cmake)
  
  ## Handle option: Activate support for Glish applications on/off

# If there is no Glish on the system, there is no use in trying to activate
# the support for Glish applications

if (NOT HAVE_GLISH)
  set (CR_GLISH_SUPPORT FALSE)
endif (NOT HAVE_GLISH)

if (CR_GLISH_SUPPORT)
  ## --- Support for Glish applications requires full CASA installation
  if (NOT HAVE_CASA)
    message (STATUS "[CR] Configuration to use CASA ...")
    include (${cr_cmake}/FindCASA.cmake)
  endif (NOT HAVE_CASA)
  ## Compiler flags for CASA
  if (HAVE_CASA)
    set (HAVE_CASACORE FALSE)
    add_definitions (-DHAVE_CASA)
  else (HAVE_CASA)
    message (STATUS "[CR] Failed to configure using CASA !")
  endif (HAVE_CASA)
  ## Compiler flags for Glish
  if (HAVE_GLISH)
    add_definitions (-DHAVE_GLISH)
  elseif (HAVE_GLISH)
    message (STATUS "[CR] Failed to locate Glish!")
    message (STATUS "     CR-Tools will be build without Glish Support!")
  endif (HAVE_GLISH)
else (CR_GLISH_SUPPORT)
  ## --- Without support for Glish applications we can/must use casacore
  message (STATUS "[CR] Configuration to use CASACORE ...")
  include (${cr_cmake}/FindCASACORE.cmake)
  if (HAVE_CASACORE)
    set (HAVE_CASA FALSE)
    add_definitions (-DHAVE_CASA)
    add_definitions (-DHAVE_CASACORE)
  endif (HAVE_CASACORE)
endif (CR_GLISH_SUPPORT)

if (HAVE_CASA OR HAVE_CASACORE)
  ## location of the header files
  include_directories (${CASA_INCLUDES})
  ## compiler flags
  add_definitions (
    -fPIC
    -ftemplate-depth-20
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
endif (HAVE_CASA OR HAVE_CASACORE)

## =============================================================================
##
##  Handling of configuration/build/install options
##
## =============================================================================

## Handle option: testing mode on/off

if (CR_ENABLE_TESTING)
  # CMake/CTest settings
  include (CTest)
  enable_testing()
  # enable building test programs
  set (CR_BUILD_TESTS ON)
  set (CR_BUILD_EXTERNAL_TESTS ON)
endif (CR_ENABLE_TESTING)

## Handle option: Activate support for Glish applications on/off


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

find_file (dataset_tbb tbb_test_file_v2.h5
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
##  Configuration for the subdirectories
##
## =============================================================================

## header files

include_directories (
  ${CR_SOURCE_DIR}/implement
  ${CR_SOURCE_DIR}/apps
  ${CR_SOURCE_DIR}/test
  )

## source code

add_subdirectory (implement)
add_subdirectory (apps)

if (CR_BUILD_EXTERNAL_TESTS)
  add_subdirectory (test)
endif (CR_BUILD_EXTERNAL_TESTS)

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

message (STATUS "+============================================================+")
message (STATUS "| CR-Tools: Summary of configuration settings                |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " CMAKE_SYSTEM ........... : ${CMAKE_SYSTEM}")
message (STATUS " CMAKE_SYSTEM_PROCESSOR . : ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " CMAKE_SYSTEM_64BIT ......: ${CMAKE_SYSTEM_64BIT}")
message (STATUS " CMAKE_SYSTEM_BIG_ENDIAN  : ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " BLITZ .................. : ${HAVE_BLITZ}")
message (STATUS " BOOST .................. : ${HAVE_BOOST}")
message (STATUS " CASA ................... : ${HAVE_CASA}")
message (STATUS " CASACORE ............... : ${HAVE_CASACORE}")
message (STATUS " CFITSIO ................ : ${HAVE_CFITSIO}")
message (STATUS " FFTW3 .................. : ${HAVE_FFTW3}")
message (STATUS " G2C .................... : ${HAVE_G2C}")
message (STATUS " GIF .................... : ${HAVE_GIF}")
message (STATUS " Glish .................. : ${HAVE_GLISH}")
message (STATUS " GSL .................... : ${HAVE_GSL}")
message (STATUS " GTK .................... : ${HAVE_GTK}")
message (STATUS " HDF5 ................... : ${HAVE_HDF5}")
message (STATUS " LAPACK ................. : ${HAVE_LAPACK}")
message (STATUS " PGPLOT ................. : ${HAVE_PGPLOT}")
message (STATUS " PLplot ................. : ${HAVE_PLPLOT}")
message (STATUS " PNG .................... : ${HAVE_PNG}")
message (STATUS " POPT ................... : ${HAVE_POPT}")
message (STATUS " PostgreSQL ............. : ${HAVE_PostgreSQL}")
message (STATUS " Python ................. : ${HAVE_PYTHON}")
message (STATUS " ROOT ................... : ${HAVE_ROOT}")
message (STATUS " Tcl / Tk  .............. : ${HAVE_TCL} / ${HAVE_TK}")
message (STATUS " WCSLIB ................. : ${HAVE_WCSLIB}")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " DAL .................... : ${HAVE_DAL}")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " Source directory ....... : ${CR_SOURCE_DIR}")
message (STATUS " Installation location .. : ${CMAKE_INSTALL_PREFIX}")
message (STATUS " Build the test programs  : ${CR_BUILD_TESTS}")
message (STATUS " Enable testing ......... : ${CR_ENABLE_TESTING}")
message (STATUS " Support for Glish ...... : ${CR_GLISH_SUPPORT}")
message (STATUS " Support for Python ..... : ${CR_PYTHON_SUPPORT}")
message (STATUS " Print debugging messages : ${CR_DEBUGGING_MESSAGES}")
message (STATUS " Verbose configuration .. : ${CR_VERBOSE_CONFIGURE}")
message (STATUS "+============================================================+")
