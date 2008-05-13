## ============================================================================== 
##
##  Register project in CMake
##
## ============================================================================== 

## Name of the project
project (StarTools)

## Minimum required CMake version
cmake_minimum_required (VERSION 2.4)

## ============================================================================== 
##
##  User selectable options
##
## ============================================================================== 

option (StarTools_VERBOSE_CONFIGURE "Verbose output during configuration?" 1)

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

## Locations in which to search for headers and libraries

set (locations_lib
  /usr/lib
  /user/local/lib
  /opt/lib
  /sw/lib
  /sw/share/lib
  ## additional locations for ROOT libraries
  /opt/root/lib
  /sw/share/root/lib
)

set (locations_includes
  /usr/include
  /user/local/include
  /opt/include
  /sw/include
  /sw/share/include
  ## additional locations for ROOT header files
  /opt/root/include
  /sw/share/root/include
)

## --- System libraries ------------------------------------

find_path (HAVE_LIBGEN_H libgen.h ${locations_includes})

if (HAVE_LIBGEN_H)
  add_definitions (-DHAVE_LIBGEN_H)
endif (HAVE_LIBGEN_H)

find_library (libm  m  ${locations_lib})
find_library (libdl dl ${locations_lib})

## --- FFTW3 -----------------------------------------------

find_library (FFTW3_LIB fftw3 PATH ${locations_lib}) 
find_path (FFTW3_INCLUDES fftw3.h PATH ${locations_includes})

if (FFTW3_INCLUDES)
  include_directories (${FFTW3_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
endif (FFTW3_INCLUDES)

## --- ROOT ------------------------------------------------

set (ROOT_ROOTSYS $ENV{ROOTSYS})

if (ROOT_ROOTSYS)
  set (ROOT_INCLUDES ${ROOT_ROOTSYS}/include)
else (ROOT_ROOTSYS)
  find_path (ROOT_INCLUDES TRandom.h PATH ${locations_includes})
endif (ROOT_ROOTSYS)

if (ROOT_INCLUDES)
  include_directories (${ROOT_INCLUDES})
  add_definitions (-DHAVE_ROOT)
endif (ROOT_INCLUDES)

foreach (lib Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore)
  find_library (root${lib} ${lib}
    PATHS ${locations_lib} ${ROOT_ROOTSYS}/lib
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

find_path (PostgreSQL_INCLUDES libpq-fe.h
  PATH ${locations_includes}
  PATH_SUFFIXES postgresql
  )

if (PostgreSQL_INCLUDES)
  include_directories (${PostgreSQL_INCLUDES})
  add_definitions (-DHAVE_POSTGRESQL)
endif (PostgreSQL_INCLUDES)

set (PostgreSQL_LIB "")

find_library (libpq pq
  PATHS ${locations_lib}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

if (libpq)
  list (APPEND PostgreSQL_LIB ${libpq})
endif (libpq)

## -------------------------------------------------------------------
## Feedback

if (StarTools_VERBOSE_CONFIGURE)
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
  message (STATUS "[Star-Tools] Summary of configuration settings")
  message (STATUS " ")
  message (STATUS "FFTW3 library       = ${FFTW3_LIB}")
  message (STATUS "FFTW3 includes      = ${FFTW3_INCLUDES}")
  message (STATUS "ROOT root directory = ${ROOT_ROOTSYS}")
  message (STATUS "ROOT library        = ${ROOT_LIBRARIES}")
  message (STATUS "ROOT includes       = ${ROOT_INCLUDES}")
  message (STATUS "PostgreSQL library  = ${PostgreSQL_LIB}")
  message (STATUS "PostgreSQL includes = ${PostgreSQL_INCLUDES}")
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
endif (StarTools_VERBOSE_CONFIGURE)

## -------------------------------------------------------------------

## ==============================================================================
##
##  
##
## ==============================================================================

include_directories (
  ${StarTools_SOURCE_DIR}/src
  ${StarTools_SOURCE_DIR}/include
  )

add_subdirectory (src)
