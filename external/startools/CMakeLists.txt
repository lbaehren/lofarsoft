## ============================================================================== 
##
##  Register project in CMake
##
## ============================================================================== 

## Name of the project
project (StarTools)

## Minimum required CMake version
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## User defined build/install options

option (StarTools_VERBOSE_CONFIGURE  "Verbose output during configuration?"       1)
option (StarTools_FORCE_BUILD        "Force rebuild and local installation"       0)
option (StarTools_BUILD_APPLICATIONS "Build all the applications in the package?" 1)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${StarTools_SOURCE_DIR}
    ${StarTools_SOURCE_DIR}/..
    ${StarTools_SOURCE_DIR}/../..
    ${StarTools_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )
  
  if (USG_ROOT)
    ## load general settings
    include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
    ## check if package is installed already
    set (StarTools_FIND_QUIETLY YES)
    include (FindWCS)
  else (USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (USG_ROOT)
endif (NOT USG_ROOT)

## ============================================================================== 
##
##  Organization of the project
##
## ============================================================================== 

#  .
#  |-- calib
#  |   |-- Amp_meas
#  |   |-- AntPosition_meas
#  |   |-- Lab_meas
#  |   |   |-- FreqResp
#  |   |   |-- HF_Bandpass_rev02
#  |   |   |-- HF_Board_17
#  |   |   |-- LNA
#  |   |   `-- RefCoef
#  |   |-- Rectifier_meas
#  |   `-- TimeDelay_meas
#  |-- doc
#  |-- include
#  |-- lib
#  |-- obj
#  |-- scripts
#  `-- src

## ==============================================================================
## 
##  Search for external packages 
##
## ==============================================================================

## --- System header files ---------------------------------

find_path (HAVE_LIBGEN_H libgen.h ${include_locations})
find_path (HAVE_TIME_H   time.h   ${include_locations})
find_path (HAVE_MATH_H   math.h   ${include_locations})

if (HAVE_LIBGEN_H)
  add_definitions (-DHAVE_LIBGEN_H)
endif (HAVE_LIBGEN_H)

if (HAVE_TIME_H)
  add_definitions (-DHAVE_TIME_H)
endif (HAVE_TIME_H)

if (HAVE_MATH_H)
  add_definitions (-DHAVE_MATH_H)
endif (HAVE_MATH_H)

## --- System libraries ------------------------------------

find_library (libm  m  ${lib_locations})
find_library (libdl dl ${lib_locations})

## --- FFTW3 -----------------------------------------------

find_library (FFTW3_LIBRARIES fftw3 PATH ${lib_locations}) 
find_path (FFTW3_INCLUDES fftw3.h PATH ${include_locations})

if (FFTW3_INCLUDES)
  include_directories (${FFTW3_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
endif (FFTW3_INCLUDES)

## --- ROOT ------------------------------------------------

set (ROOT_ROOTSYS $ENV{ROOTSYS})

if (ROOT_ROOTSYS)
  set (ROOT_INCLUDES ${ROOT_ROOTSYS}/include)
else (ROOT_ROOTSYS)
  find_path (ROOT_INCLUDES TRandom.h PATH ${include_locations})
endif (ROOT_ROOTSYS)

if (ROOT_INCLUDES)
  include_directories (${ROOT_INCLUDES})
  add_definitions (-DHAVE_ROOT)
endif (ROOT_INCLUDES)

foreach (lib Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread freetype Gui)
  find_library (root${lib} ${lib}
    PATHS ${lib_locations} ${ROOT_ROOTSYS}/lib
    )
  if (root${lib})
    list (APPEND ROOT_LIBRARIES ${root${lib}})
  endif (root${lib})
endforeach (lib)

if (NOT ROOT_INCLUDES)
  message (SEND_ERROR "Missing ROOT header files!")
  set (HAVE_ROOT FALSE)
endif (NOT ROOT_INCLUDES)

if (NOT ROOT_LIBRARIES)
  message (SEND_ERROR "Missing ROOT libraries!")
  set (HAVE_ROOT FALSE)
endif (NOT ROOT_LIBRARIES)

## --- PostgreSQL ------------------------------------------

find_path (POSTGRESQL_INCLUDES libpq-fe.h
  PATH ${include_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  )

find_library (POSTGRESQL_LIBRARIES pq
  PATHS ${lib_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  NO_DEFAULT_PATH
  )

if (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)
  set (HAVE_PostgreSQL TRUE)
  include_directories (${POSTGRESQL_INCLUDES})
  add_definitions (-DHAVE_POSTGRESQL)
endif (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)

## ==============================================================================
##
##  Installation settings
##
## ==============================================================================

## Prefix for the installation will be dependent on where the package is build;
## if the package is embedded within the directory structure of the USG software,
## we install into "release". Otherwise we use the "include" and "lib" directories
## within the package directory itself.

find_path (prefix release_area.txt
  PATHS
  ${StarTools_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
else (prefix)
  set (CMAKE_INSTALL_PREFIX ${StarTools_SOURCE_DIR})
endif (prefix)

## No relinking and full RPATH for the install tree

set (CMAKE_SKIP_BUILD_RPATH TRUE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


## ==============================================================================
##
##  Subdirectories of the project  
##
## ==============================================================================

include_directories (
  ${StarTools_SOURCE_DIR}/src
  ${StarTools_SOURCE_DIR}/include
  )

add_subdirectory (src)

## ==============================================================================
##
##  Summary of configuration
##
## ==============================================================================

if (StarTools_VERBOSE_CONFIGURE)
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
  message (STATUS "[Star-Tools] Summary of configuration settings")
  message (STATUS " ")
  message (STATUS "FFTW3 library       = ${FFTW3_LIBRARIES}")
  message (STATUS "FFTW3 includes      = ${FFTW3_INCLUDES}")
  message (STATUS "ROOT root directory = ${ROOT_ROOTSYS}")
  message (STATUS "ROOT library        = ${ROOT_LIBRARIES}")
  message (STATUS "ROOT includes       = ${ROOT_INCLUDES}")
  message (STATUS "PostgreSQL library  = ${POSTGRESQL_LIBRARIES}")
  message (STATUS "PostgreSQL includes = ${POSTGRESQL_INCLUDES}")
  message (STATUS " ")
  message (STATUS "Installation prefix = ${CMAKE_INSTALL_PREFIX}")
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
endif (StarTools_VERBOSE_CONFIGURE)

