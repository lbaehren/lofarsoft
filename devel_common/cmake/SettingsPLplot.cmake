
## -------------------------------------------------------------------
## Search for external components

find_path (plplot_cmake CMakeSettings.cmake
  PATHS 
  .
  ./..
  ./../..
  ./../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (plplot_cmake)
  list (APPEND CMAKE_MODULE_PATH ${plplot_cmake})
else (plplot_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (plplot_cmake)

## Python.Numeric

include (${plplot_cmake}/FindNumeric.cmake)

if (NUMERIC_INCLUDES)
  include_directories (${NUMERIC_INCLUDES})
endif (NUMERIC_INCLUDES)

## -------------------------------------------------------------------
## Language bindings

## building of dynamic libraries needs to be enabled to create Python
## bindings; otherwise the Python bindings are disabled
set (BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries?")

set (ENABLE_f77 OFF CACHE BOOL "Enable bindings for Fortran 77?")
set (ENABLE_f95 OFF CACHE BOOL "Enable bindings for Fortran 95?")
set (ENABLE_gnome2 OFF CACHE BOOL "Enable bindings for GNOME2?")
set (ENABLE_itcl OFF CACHE BOOL "Enable bindings for [incr Tcl]?")
set (ENABLE_itk OFF CACHE BOOL "Enable bindings for [incr Tk]?")
set (ENABLE_python ON CACHE BOOL "Enable bindings for Python?")
set (ENABLE_java OFF CACHE BOOL "Enable bindings for Java?")
set (ENABLE_pygcw OFF CACHE BOOL "Enable bindings for Python.gcw?")

## -------------------------------------------------------------------
## Device drivers

set (ENABLE_DYNDRIVERS OFF CACHE BOOL "Enable dynamic drivers?")

set (PLD_aqt OFF CACHE BOOL "Enable driver aqt")
set (PLD_hp7470 OFF CACHE BOOL "Enable driver hp7470")
set (PLD_hp7580 OFF CACHE BOOL "Enable driver hp7580")
set (PLD_lj_hpgl OFF CACHE BOOL "Enable driver lj_hpgl")
set (PLD_svg OFF CACHE BOOL "Enable driver svg")

if (APPLE)
  set (PLD_wxwidgets OFF CACHE BOOL "Enable driver wxwidgets")
endif (APPLE)

## -------------------------------------------------------------------
## Testing

set (BUILD_TEST ON CACHE BOOL "Build test programs?")

## -------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package PLplot.")
  get_filename_component (tmp ${prefix} ABSOLUTE)
  set (CMAKE_INSTALL_PREFIX ${tmp} CACHE STRING "Installation prefix") 
  ## Additional setting for Python bindings
  set (PYTHON_INSTDIR "${prefix}/lib/python")
endif (prefix)

set (CMAKE_INSTALL_BINDIR "bin")
set (CMAKE_INSTALL_DATADIR "share")
set (CMAKE_INSTALL_EXEC_PREFIX "bin")
set (CMAKE_INSTALL_INCLUDEDIR "include")
set (CMAKE_INSTALL_INFODIR "share/info")
set (CMAKE_INSTALL_LIBDIR "lib")
set (CMAKE_INSTALL_MANDIR "share/man")

if (UNIX)
  set (CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif (UNIX)
