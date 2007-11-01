
## ------------------------------------------------------------------------------
## Additional settings to build and install PLplot

## -------------------------------------------------------------------
## General settings for the drivers

set (BUILD_SHARED_LIBS "0" CACHE BOOL "Build shared libraries?")
set (ENABLE_DYNDRIVERS "0" CACHE BOOL "Enable dynamic drivers?")
set (ENABLE_f77 OFF CACHE BOOL "Enable drivers for Fortran 77?")
set (ENABLE_f95 OFF CACHE BOOL "Enable drivers for Fortran 95?")
set (ENABLE_gnome2 OFF CACHE BOOL "Enable drivers for GNOME2?")
set (ENABLE_itcl OFF CACHE BOOL "Enable drivers for [incr Tcl]?")
set (ENABLE_itk OFF CACHE BOOL "Enable drivers for [incr Tk]?")

## -------------------------------------------------------------------
## disable individual drivers

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
endif (prefix)

set (CMAKE_INSTALL_BINDIR "bin")
set (CMAKE_INSTALL_DATADIR "share")
set (CMAKE_INSTALL_EXEC_PREFIX "bin")
set (CMAKE_INSTALL_INCLUDEDIR "include")
set (CMAKE_INSTALL_INFODIR "share/info")
set (CMAKE_INSTALL_LIBDIR "lib")
set (CMAKE_INSTALL_MANDIR "share/man")
