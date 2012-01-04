
## Name of the project
project (PULSAR C CXX Fortran)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${PULSAR_SOURCE_DIR}
    ${PULSAR_SOURCE_DIR}/..
    ${PULSAR_SOURCE_DIR}/../..
    ${PULSAR_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

if (LUS_SOURCE_DIR)
  include (${LUS_SOURCE_DIR}/devel_common/cmake/CMakeSettings.cmake)
  list (INSERT CMAKE_MODULE_PATH 0 ${LUS_SOURCE_DIR}/devel_common/cmake )
else (LUS_SOURCE_DIR)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_SOURCE_DIR)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (PULSAR_ENABLE_TESTING     "Enable generation and running of tests?"    YES )
option (PULSAR_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"  YES )
option (PULSAR_DEBUGGING_MESSAGES "Print debugging information?"               YES )
option (PULSAR_VERBOSE_CONFIGURE  "Verbose output during configuration?"       YES )

##__________________________________________________________
## Handle option: testing mode on/off

if (PULSAR_ENABLE_TESTING)
  ## Include CTest module
  include (CTest)
  enable_testing()
endif (PULSAR_ENABLE_TESTING)

##__________________________________________________________
## Handle option: verbose mode during configuration on/off

if (NOT PULSAR_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY     YES )
  set (GFORTRAN_FIND_QUIETLY    YES )
  set (FFTW3_FIND_QUIETLY       YES )
  set (GLIB_FIND_QUIETLY        YES )
  set (MPI_FIND_QUIETLY         YES )
  set (NUMPY_FIND_QUIETLY       YES )
  set (PGPLOT_FIND_QUIETLY      YES )
  set (PYTHON_FIND_QUIETLY      YES )
  set (X11_FIND_QUIETLY         YES )
endif (NOT PULSAR_VERBOSE_CONFIGURE)

##__________________________________________________________
## Handle option: debugging messages on/off

if (PULSAR_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (PULSAR_DEBUGGING_MESSAGES)

## ==============================================================================
##
##                                                              System inspection
##
## ==============================================================================

## Determine which value to assign to the architecture flag

if (UNIX)
  if (APPLE)
    set (PULSAR_LOGIN_ARCH "darwin")
  else (APPLE)
    set (PULSAR_LOGIN_ARCH "linux")
  endif (APPLE)
endif (UNIX)

## ==============================================================================
##
##                                                             Installation setup
##
##  Installation settings need to be processed early on, as this will have an 
##  effect on the locations later on inspected by the find modules; if we want
##  CMake to be able to locate the proper custom version (of libraries such as
##  e.g. FFTW3), we need to point it to the correct location -- hence we need to
##  set this up first.
##
## ==============================================================================

## Load the module definition the installation setup

include (${PULSAR_SOURCE_DIR}/cmake/InstallationSetup.cmake)

## Use the installation setup to guide CMake's search modules

list (INSERT CMAKE_INCLUDE_PATH 0 ${CMAKE_INSTALL_PREFIX}/include )
list (INSERT CMAKE_LIBRARY_PATH 0 ${CMAKE_INSTALL_PREFIX}/lib )
 
## ==============================================================================
##
##  External dependencies
##
## ==============================================================================

set (NO_DEFAULT_PATH YES)

## Additional locations

foreach (_extraLocations
    /home/joerivl/src/presto/deps
    )

  if (EXISTS ${_extraLocations})
    ## Augment search locations
    list (INSERT lib_locations     0 ${_extraLocations})
    list (INSERT include_locations 0 ${_extraLocations})
    ## Augment include path
    include_directories (${_extraLocations})
  endif (EXISTS ${_extraLocations})
  
endforeach (_extraLocations)

##__________________________________________________________
## System programs (tar, gzip, ...)

include (FindUnixCommands)

##__________________________________________________________
## System header files

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

find_library (libdl      dl      )
find_library (libm       m       )
find_library (libpthread pthread )
find_library (libutil    util    )
find_library (libz       z       )

##__________________________________________________________
## Type sizes

include (CheckTypeSize)

check_type_size ("long"          SIZEOF_LONG       )
check_type_size ("long int"      SIZEOF_LONG_INT   )
check_type_size ("long long"     SIZEOF_LONG_LONG  )
check_type_size ("uint"          SIZEOF_UINT       )
check_type_size ("int32_t"       SIZEOF_INT32_T    )
check_type_size ("int64_t"       SIZEOF_INT64_T    )
check_type_size ("uint32_t"      SIZEOF_UINT32_T   )
check_type_size ("uint64_t"      SIZEOF_UINT64_T   )

##__________________________________________________________
## External packages

set (FIND_CFITSIO_CMAKE FALSE)
include (${PULSAR_SOURCE_DIR}/cmake/FindPythonPath.cmake)

foreach (pulsarExternalPackages
    CFITSIO
    FFTW3
    GLIB
    GFortran
    MPI
    Numeric
    NumPy
    PGPLOT
    Python
    )

  ##
  ## Check if there is a custom version; if yes, rather use the custom
  ## version instead of the one provided by either LUS or CMake.
  ##
  if (EXISTS ${PULSAR_SOURCE_DIR}/cmake/Find${pulsarExternalPackages}.cmake)
    ## Load custom module
    include (${PULSAR_SOURCE_DIR}/cmake/Find${pulsarExternalPackages}.cmake)
  else (EXISTS ${PULSAR_SOURCE_DIR}/cmake/Find${pulsarExternalPackages}.cmake)
    ## Load LUS or standard module
    find_package (${pulsarExternalPackages})
  endif (EXISTS ${PULSAR_SOURCE_DIR}/cmake/Find${pulsarExternalPackages}.cmake)
  
endforeach (pulsarExternalPackages)

##__________________________________________________________
## X11 

list (APPEND lib_locations /usr/pkg/xorg/lib)
list (APPEND lib_locations /usr/X11R6/lib)
list (APPEND lib_locations /usr/X11R7/lib)
list (APPEND lib_locations /usr/openwin/lib)
list (APPEND lib_locations /opt/local/lib)
list (APPEND lib_locations /usr/X11/lib)
list (APPEND lib_locations /usr/lib)
list (APPEND lib_locations /usr/lib64)

if (NOT HAVE_X11)
  include (FindX11)
endif (NOT HAVE_X11)

if (NOT X11_Xt_LIB)
  ## search for the library
  find_library (libXt Xt ${lib_locations})
  ## add the library to the list of X11 libraries
  if (libXt)
    message (STATUS "Found libXt = ${libXt}")
    set (X11_Xt_LIB ${libXt} CACHE FILEPATH "Path to libXt." FORCE)
    list (APPEND X11_LIBRARIES ${libXt})
  else (libXt)
    message (STATUS "Unable to find libXt!")
  endif (libXt)
endif (NOT X11_Xt_LIB)

if (NOT X11_X11_LIB)
  ## search for the library
  find_library (libX11 X11 ${lib_locations})
  ## add the library to the list of X11 libraries
  if (libX11)
    message (STATUS "Found libX11 = ${libX11}")
    set (X11_X11_LIB ${libX11} CACHE FILEPATH "Path to libX11." FORCE)
    list (APPEND X11_LIBRARIES ${libX11})
  else (libX11)
    message (STATUS "Unable to find libX11!")
  endif (libX11)
endif (NOT X11_X11_LIB)

if (NOT X11_INCLUDE_DIR)
  find_path (X11_INCLUDE_DIR X11/xpm.h X11/Xutil.h
    PATHS
    ${include_locations}
    )
  if (X11_INCLUDE_DIR)
    set (X11_X11_INCLUDE_PATH ${X11_INCLUDE_DIR})
  endif (X11_INCLUDE_DIR)
endif (NOT X11_INCLUDE_DIR)

##__________________________________________________________
## FFTW3 / FFTW3F

## If possible, use the static library; however if only a
## shared version is available, use this as backup
if (FFTW3_FFTW3_STATIC_LIBRARY)
  set (FFTW3_LIBRARIES ${FFTW3_FFTW3_STATIC_LIBRARY} CACHE FILEPATH
    "Path to FFTW3 library" FORCE)
else (FFTW3_FFTW3_STATIC_LIBRARY)
  if (FFTW3_FFTW3_LIBRARY)
    set (FFTW3_LIBRARIES ${FFTW3_FFTW3_LIBRARY}} CACHE FILEPATH
      "Path to FFTW3 library" FORCE)
  endif (FFTW3_FFTW3_LIBRARY)
endif (FFTW3_FFTW3_STATIC_LIBRARY)

## -----------------------------------------------------------------------------
## Include directories

## header files of the PULSAR itself

include_directories (
  ${PULSAR_SOURCE_DIR}/test
  ${PULSAR_SOURCE_DIR}/apps
  )

## header files of external packages

if (HAVE_CFITSIO)
  ## Add include directory
  include_directories (${CFITSIO_INCLUDES})
  ## Add compiler flag
  add_definitions (-DHAVE_CFITSIO)
  ## Extract library path
  get_filename_component (CFITSIO_LIBRARY_DIR ${CFITSIO_LIBRARIES} PATH)
endif (HAVE_CFITSIO)

if (HAVE_GLIB)
  include_directories (${GLIB_INCLUDES})
  add_definitions (-DHAVE_GLIB)
else (HAVE_GLIB)
  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of GLIB!")
endif (HAVE_GLIB)

if (NOT GFORTRAN_LIBRARY)
  message (WARNING "[PULSAR] Missing or incomplete installation of GFORTRAN!")
endif (NOT GFORTRAN_LIBRARY)

if (GFORTRAN_EXECUTABLE)
  set (CMAKE_Fortran_COMPILER ${GFORTRAN_EXECUTABLE})
endif (GFORTRAN_EXECUTABLE)

if (HAVE_PGPLOT)
  include_directories (${PGPLOT_INCLUDES})
  add_definitions (-DHAVE_PGPLOT)
  set (PGPLOT_DIR ${PGPLOT_LIBRARY_DIR})
  set (PGPLOT_INCLUDES ${PGPLOT_INCLUDES})
  set(ENV{PGPLOT_DIR} ${PGPLOT_LIBRARY_DIR})
  set(ENV{PGPLOT_INCLUDES} ${PGPLOT_INCLUDES})
  message (STATUS "[PULSAR] Setting environment variables PGPLOT_INCLUDES to $ENV{PGPLOT_INCLUDES}!")
  message (STATUS "[PULSAR] Setting environment variables PGPLOT_DIR to $ENV{PGPLOT_DIR}!")
  EXECUTE_PROCESS( COMMAND bash -c "echo PGPLOT_DIR=$PGPLOT_DIR" )
  EXECUTE_PROCESS( COMMAND bash -c "echo PGPLOT_INCLUDES=$PGPLOT_INCLUDES" )
else (HAVE_PGPLOT)
  message (SEND_ERROR "[PULSAR] Missing or incomplete installation of PGPLOT!")
endif (HAVE_PGPLOT)

if (MPI_FOUND)
  include_directories (${MPI_INCLUDE_PATH})
  add_definitions (-DHAVE_MPI)
else (MPI_FOUND)
  message (STATUS "[PULSAR] Missing or incomplete installation of MPI!")
endif (MPI_FOUND)

if (PYTHON_FOUND) 
  include_directories (${PYTHON_INCLUDES})
else (PYTHON_FOUND) 
  set (PULSAR_PYTHON_BINDINGS FALSE)
endif (PYTHON_FOUND)

if (NUMPY_INCLUDES) 
  include_directories (${NUMPY_INCLUDES} ${NUMPY_INCLUDES}/numpy)
else (NUMPY_INCLUDES) 
  set (PULSAR_PYTHON_BINDINGS FALSE)
endif (NUMPY_INCLUDES)

if (NUMERIC_INCLUDES) 
  include_directories (${NUMERIC_INCLUDES} ${NUMERIC_INCLUDES}/Numeric)
else (NUMERIC_INCLUDES) 
  set (PULSAR_PYTHON_BINDINGS FALSE)
endif (NUMERIC_INCLUDES)

if (X11_INCLUDE_DIR)
  include_directories (${X11_INCLUDE_DIR})
else (X11_INCLUDE_DIR)
  message (STATUS "[PULSAR] Missig X11 include directory!")
endif (X11_INCLUDE_DIR)

if (HAVE_FFTW3 OR FFTW3_FOUND)
  include_directories (${FFTW3_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
else (HAVE_FFTW3 OR FFTW3_FOUND)
  message (STATUS "[PULSAR] Missing or incomplete installation of FFTW3 (not the same as FFTW3F)!")
endif (HAVE_FFTW3 OR FFTW3_FOUND)

## =============================================================================
##
##  Subdirectories
##
## =============================================================================

add_subdirectory (scripts)
add_subdirectory (apps)
add_subdirectory (external)

if (PULSAR_BUILD_TESTS)
  add_subdirectory (test)
endif (PULSAR_BUILD_TESTS)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

include (${PULSAR_SOURCE_DIR}/cmake/ConfigurationSummary.cmake)
