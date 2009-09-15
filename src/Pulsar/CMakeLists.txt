##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 2981 2009-09-03 10:34:56Z baehren                      $
##------------------------------------------------------------------------------

## Name of the project
project (PULSAR)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PULSAR_SOURCE_DIR}
  ${PULSAR_SOURCE_DIR}/..
  ${PULSAR_SOURCE_DIR}/../..
  ${PULSAR_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (PULSAR_ENABLE_TESTING     "Enable generation and running of tests?"    NO  )
option (PULSAR_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"  YES )
option (PULSAR_DEBUGGING_MESSAGES "Print debugging information?"               YES )
option (PULSAR_VERBOSE_CONFIGURE  "Verbose output during configuration?"       YES )

## Handle option: testing mode on/off

if (PULSAR_ENABLE_TESTING)
  ## Include CTest module
  include (CTest)
  enable_testing()
endif (PULSAR_ENABLE_TESTING)

## Handle option: verbose mode during configuration on/off

if (NOT PULSAR_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY     YES )
  set (GFORTRAN_FIND_QUIETLY    YES )
  set (GLIB_FIND_QUIETLY        YES )
  set (PGPLOT_FIND_QUIETLY      YES )
  set (NUMPY_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY      YES )
endif (NOT PULSAR_VERBOSE_CONFIGURE)

## Handle option: debugging messages on/off

if (PULSAR_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (PULSAR_DEBUGGING_MESSAGES)

## ==============================================================================
##
##  External dependencies
##
## ==============================================================================

set (NO_DEFAULT_PATH YES)

find_program (tar_executable  tar  ${bin_locations} )
find_program (gzip_executable gzip ${bin_locations} )

##__________________________________________________________
## System header files

find_path (HAVE_LIBGEN_H     libgen.h      PATHS ${include_locations} )
find_path (HAVE_STDINT_H     stdint.h      PATHS ${include_locations} )
find_path (HAVE_STDLIB_H     stdlib.h      PATHS ${include_locations} )
find_path (HAVE_STRING_H     string.h      PATHS ${include_locations} )
find_path (HAVE_STRINGS_H    strings.h     PATHS ${include_locations} )
find_path (HAVE_SYS_STAT_H   sys/stat.h    PATHS ${include_locations} )
find_path (HAVE_SYS_TIME_H   sys/time.h    PATHS ${include_locations} )
find_path (HAVE_SYS_TYPES_H  sys/types.h   PATHS ${include_locations} )
find_path (HAVE_SYS_UTIME_H  sys/utime.h   PATHS ${include_locations} )

##__________________________________________________________
## System Libraries

find_library (libdl      dl       ${lib_locations})
find_library (libm       m        ${lib_locations})
find_library (libpthread pthread  ${lib_locations})
find_library (libutil    util     ${lib_locations})
find_library (libz       z        ${lib_locations})

##__________________________________________________________
## Type sizes

include (CheckTypeSize)

check_type_size ("short"         SIZEOF_SHORT       )
check_type_size ("int"           SIZEOF_INT         )
check_type_size ("float"         SIZEOF_FLOAT       )
check_type_size ("double"        SIZEOF_DOUBLE      )
check_type_size ("long"          SIZEOF_LONG        )
check_type_size ("long int"      SIZEOF_LONG_INT    )
check_type_size ("long long"     SIZEOF_LONG_LONG   )
check_type_size ("uint"          SIZEOF_UINT        )
check_type_size ("int32_t"       SIZEOF_INT32_T     )
check_type_size ("int64_t"       SIZEOF_INT64_T     )
check_type_size ("uint32_t"      SIZEOF_UINT32_T     )
check_type_size ("uint64_t"      SIZEOF_UINT64_T     )

##__________________________________________________________
## CFITSIO

if (NOT HAVE_CFITSIO)
  include (FindCFITSIO)
endif (NOT HAVE_CFITSIO)

##__________________________________________________________
## GLIB library

if (NOT HAVE_GLIB)
  include (FindGLIB)
endif (NOT HAVE_GLIB)

##__________________________________________________________
## PGPLOT library

if (NOT HAVE_PGPLOT)
  include (FindPGPLOT)
endif (NOT HAVE_PGPLOT)

##__________________________________________________________
## GFortran library

if (NOT HAVE_GFORTRAN)
  include (FindGFortran)
endif (NOT HAVE_GFORTRAN)

##__________________________________________________________
## PYTHON 

if (NOT HAVE_PYTHON)
  include (FindPython)
  include (FindNumPy)
endif (NOT HAVE_PYTHON)

## -----------------------------------------------------------------------------
## Include directories

## header files of the PULSAR itself

include_directories (
  ${PULSAR_SOURCE_DIR}/test
  ${PULSAR_SOURCE_DIR}/apps
  )

## header files of external packages

if (HAVE_CFITSIO)
  include_directories (${CFITSIO_INCLUDES})
  add_definitions (-DHAVE_CFITSIO)
else (HAVE_CFITSIO)
  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of CFITSIO!")
endif (HAVE_CFITSIO)

if (HAVE_GLIB)
  include_directories (${GLIB_INCLUDES})
  add_definitions (-DHAVE_GLIB)
else (HAVE_GLIB)
  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of GLIB!")
endif (HAVE_GLIB)

if (HAVE_GFORTRAN)
  include_directories (${GFORTRAN_INCLUDES})
  add_definitions (-DHAVE_GFORTRAN)
else (HAVE_GFORTRAN)
##  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of GFORTRAN!")
  message (STATUS "[PULSAR] Missing or incomplete installation of GFORTRAN!")
endif (HAVE_GFORTRAN)

if (HAVE_PGPLOT)
  include_directories (${PGPLOT_INCLUDES})
  add_definitions (-DHAVE_PGPLOT)
  set (PGPLOT_DIR ${PGPLOT_LIBRARY_DIR})
else (HAVE_PGPLOT)
  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of PGPLOT!")
endif (HAVE_PGPLOT)

if (NOT HAVE_PYTHON) 
  set (PULSAR_PYTHON_BINDINGS FALSE)
  message (STATUS "[PULSAR] Unable to generate Python bindings; missing Python headers!")
endif (NOT HAVE_PYTHON)

if (NOT NUMPY_INCLUDES) 
  set (PULSAR_PYTHON_BINDINGS FALSE)
  message (STATUS "[PULSAR] Unable to generate Python bindings; missing NumPy headers!")
endif (NOT NUMPY_INCLUDES)

## -----------------------------------------------------------------------------
## Installation - We install into $LOFARSOFT/release/share/pulsar to keep all
## the components in one place but not have them interfere with possible other
## installations of the same packages (e.g. FFTW3)

find_path (prefix release_area.txt
  PATHS
  ${PULSAR_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package PULSAR.")
  set (CMAKE_INSTALL_PREFIX ${prefix}/share/pulsar) 
endif (prefix)

# skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH YES)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH YES)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH YES)

## =============================================================================
##
##  Subdirectories
##
## =============================================================================

add_subdirectory (external)
add_subdirectory (apps)

if (PULSAR_BUILD_TESTS)
  add_subdirectory (test)
endif (PULSAR_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Feedback

if (PULSAR_VERBOSE_CONFIGURE)

message (STATUS "+============================================================+")
message (STATUS "| PULSAR: Summary of configuration settings                  |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " Enable test programs       = ${PULSAR_ENABLE_TESTING}")
message (STATUS " Enable compiler warnings   = ${PULSAR_COMPILER_WARNINGS}")
message (STATUS " Print debugging messages   = ${PULSAR_DEBUGGING_MESSAGES}")
message (STATUS " Enable verbose configure   = ${PULSAR_VERBOSE_CONFIGURE}")
message (STATUS "+------------------------------------------------------------+")

endif (PULSAR_VERBOSE_CONFIGURE)
