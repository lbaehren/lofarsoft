
## ------------------------------------------------------------------------------
## Additional settings to build and install PLplot

## General settings for the drivers

set (BUILD_SHARED_LIBS OFF)
set (ENABLE_DYNDRIVERS OFF)

## disable individual drivers

set (PLD_aqt OFF)
set (PLD_hp7470 OFF)
set (PLD_hp7580 OFF)
set (PLD_lj_hpgl OFF)
set (PLD_svg OFF)

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
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

set (CMAKE_INSTALL_BINDIR "bin")
set (CMAKE_INSTALL_DATADIR "share")
set (CMAKE_INSTALL_EXEC_PREFIX "bin")
set (CMAKE_INSTALL_INCLUDEDIR "include")
set (CMAKE_INSTALL_INFODIR "share/info")
set (CMAKE_INSTALL_LIBDIR "lib")
set (CMAKE_INSTALL_MANDIR "share/man")
