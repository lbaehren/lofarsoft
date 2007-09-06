##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (pgplot C Fortran)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## User defined build/install options

option (pgplot_ENABLE_TESTING "Enable testing environment?" 1)
option (pgplot_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)

## Additional CMake components

include (CPack)

## Handle option: testing mode on/off

if (pgplot_ENABLE_TESTING)
  ## include CMake module
  include (CTest)
  ## enable testing mode
  enable_testing()
  ## add directory containing the test programs and examples
  add_subdirectory (examples)
endif (pgplot_ENABLE_TESTING)

## Handle option: verbose during configuration on/off

if (NOT pgplot_VERBOSE_CONFIGURE)
  set (G2C_FIND_QUIETLY 1)
  set (GIF_FIND_QUIETLY 1)
  set (JPEG_FIND_QUIETLY 1)
  set (PNG_FIND_QUIETLY 1)
  set (TCL_FIND_QUIETLY 1)
  set (TK_FIND_QUIETLY 1)
  set (X11_FIND_QUIETLY 1)
  set (XAW_FIND_QUIETLY 1)
  set (XM_FIND_QUIETLY 1)
  set (ZLIB_FIND_QUIETLY 1)
endif (NOT pgplot_VERBOSE_CONFIGURE)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (lib_locations
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /sw/lib
  )

set (include_locations
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for required external libraries

find_package (JPEG)
find_package (X11)
find_package (ZLIB)

## check where to find the CMake scripts

find_path (pgplot_cmake FindG2C.cmake FindGIF.cmake
  PATHS 
  ${pgplot_SOURCE_DIR}
  ${pgplot_SOURCE_DIR}/..
  ${pgplot_SOURCE_DIR}/../..
  ${pgplot_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (pgplot_cmake)
  include (${pgplot_cmake}/FindG2C.cmake)
  include (${pgplot_cmake}/FindGIF.cmake)
  include (${pgplot_cmake}/FindPNG.cmake)
  include (${pgplot_cmake}/FindTCL.cmake)
  include (${pgplot_cmake}/FindTK.cmake)
  include (${pgplot_cmake}/FindXaw.cmake)
  include (${pgplot_cmake}/FindXm.cmake)
endif (pgplot_cmake)

## Additional X11 components which might not be picked up otherwise

find_library (libXt Xt PATHS ${lib_locations})

## ------------------------------------------------------------------------------
## Create list of drivers to be included into the library

set (pgplot_external_includes "")
set (pgplot_external_libraries "")

## list of sources files providing drivers

set (pgplot_drivers
  ./drivers/nudriv.f
  ./drivers/pgdriv.f
  ./drivers/pgxwin.c
  ./drivers/psdriv.f
  ./drivers/wddriv.f
  ./drivers/xmdriv.c
  ./drivers/xwdriv.c
  )

## set the name of file containing the list of drivers

set (outfile "build/drivers.list")

file (WRITE ${outfile} "! List of selected drivers\n\n")

## [1] Null device -------------------------------

file (APPEND ${outfile} "NUDRIV 0 /NULL     Null device	 Std F77\n")

## [2] PGPLOT metafile ----------------------------

file (APPEND ${outfile} "PGDRIV 0 /PGMF     PGPLOT metafile  Std F77\n")

## [3] Postscript --------------------------------

file (APPEND ${outfile} "PSDRIV 1 /PS       PostScript printers, monochrome, landscape	Std F77\n")
file (APPEND ${outfile} "PSDRIV 2 /VPS      Postscript printers, monochrome, portrait		Std F77\n")
file (APPEND ${outfile} "PSDRIV 3 /CPS      PostScript printers, color, landscape		Std F77\n")
file (APPEND ${outfile} "PSDRIV 4 /VCPS     PostScript printers, color, portrait		Std F77\n")

## [4] X-window Rivet/Tk -------------------------

file (APPEND ${outfile} "WDDRIV 1 /WD       X Window dump file, landscape\n")
file (APPEND ${outfile} "WDDRIV 2 /VWD      X Window dump file, portrait\n")

## [5] Motif applications containing XmPgplot

if (HAVE_XM)
  file (APPEND ${outfile} "XMDRIV 0 /XMOTIF   Motif applications containing XmPgplot widgets.   C\n")
  ## update driver list
  list (APPEND pgplot_drivers ./drivers/xmotif/XmPgplot.c)
  ## update compiler list
  list (APPEND pgplot_external_includes ${XM_INCLUDES})
  ## update linker list
  list (APPEND pgplot_external_libraries ${XM_LIBRARIES})
endif (HAVE_XM)

## [6] X Window System

if (XAW_LIBRARIES AND libXt)
  file (APPEND ${outfile} "XWDRIV 1 /XWINDOW  Workstations running X Window System		C\n")
  file (APPEND ${outfile} "XWDRIV 2 /XSERVE   Persistent window on X Window System		C\n")
  ## update linker list
  list (APPEND pgplot_external_libraries ${XAW_LIBRARIES})
  list (APPEND pgplot_external_libraries ${libXt})
endif (XAW_LIBRARIES AND libXt)

## [7] GIF-format file

if (GIF_LIBRARIES)
 file (APPEND ${outfile} "GIDRIV 1 /GIF      GIF-format file, landscape\n")
 file (APPEND ${outfile} "GIDRIV 2 /VGIF     GIF-format file, portrait\n")
 ## update driver list
 list (APPEND pgplot_drivers ./drivers/gidriv.f)
 ## update linker list
 list (APPEND pgplot_external_libraries ${GIF_LIBRARIES})
endif (GIF_LIBRARIES)

## [8] Portable Network Graphics file

if (PNG_LIBRARIES)
  file (APPEND ${outfile} "PNDRIV 1 /PNG      Portable Network Graphics file                 C\n")
  file (APPEND ${outfile} "PNDRIV 2 /TPNG     Portable Network Graphics file - transparent background C\n")
  ## update compiler instructions
  list (APPEND pgplot_external_includes ${PNG_INCLUDES})
  ## update linker instructions
  list (APPEND pgplot_external_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (HAVE_TCL AND HAVE_TK)
  file (APPEND ${outfile} "TKDRIV 0 /XTK      X-window Tcl/Tk programs with pgplot widgets.  C\n")
  file (APPEND ${outfile} "RVDRIV 0 /XRV       X-window Rivet/Tk programs with pgplot widgets.   C")
  ## update driver list
  list (APPEND pgplot_drivers ./drivers/rvdriv.c)
  list (APPEND pgplot_drivers ./drivers/tkdriv.c)
  list (APPEND pgplot_drivers ./drivers/xtk/tkpgplot.c)
  ## update compiler instructions
  list (APPEND pgplot_external_includes ${TCL_INCLUDES})
  ## update linker instructions
  list (APPEND pgplot_external_libraries ${TCL_LIBRARIES})
  list (APPEND pgplot_external_libraries ${TK_LIBRARIES})
endif (HAVE_TCL AND HAVE_TK)

## ------------------------------------------------------------------------------
## Start building the library
##
## The ``standard'' routines pgplot/src/pg*.f and pgplot/src/gr*.f. All of these
## routines should be compiled and put in the object-module library, although the
## ``obsolete'' routines may be omitted (they are not used by current PGPLOT
## programs and will not be included in future versions of PGPLOT). The obsolete
## routines are: grchar, grchr0, grdat2, grgtc0, grinqfont, grinqli, grinqpen,
## grlinr, grmark, grmovr, grsetfont, grsetli, grsetpen, grtran, grvect, pgsetc,
## pgsize.
## 
## Required PGPLOT sub-directories.
## pgplot                          ... SRC
## |-- applications
## |   |-- curvefit
## |   |-- plot10
## |   `-- plotpg
## |-- cpg
## |-- drivers                     ... DRVDIR
## |   |-- old
## |   |-- xathena
## |   |-- xmotif
## |   `-- xtk
## |-- examples                    ... DEMDIR
## |-- fonts                       ... FNTDIR
## |-- pgdispd
## |-- pgmf
## |-- src                         ... SRCDIR
## |-- sys                         ... GENDIR
## 
## DEFAULT_TARGETS="lib grfont.dat prog pgplot.doc"
##

## subdirectories to consider

add_subdirectory (drivers)
add_subdirectory (fonts)
add_subdirectory (src)
add_subdirectory (sys)
add_subdirectory (cpg)

## include directories

include_directories (
  ${pgplot_SOURCE_DIR}
  ${pgplot_SOURCE_DIR}/cpg
  ${pgplot_SOURCE_DIR}/drivers
  ${pgplot_SOURCE_DIR}/drivers/xmotif
  ${pgplot_SOURCE_DIR}/src
  ${pgplot_SOURCE_DIR}/sys
  ${pgplot_modules}
  ${pgplot_external_includes}
  ${X11_INCLUDE_DIR}
)

FILE (GLOB pgplot_src ./src/*.f ./src/*.c)
FILE (GLOB pgplot_sys ./sys/*.f ./sys/*.c)
FILE (GLOB pgplot_fonts ./fonts/*.f)

## -- libpgplot ----------------------------------
##
## libpgplot.a : $(PG_ROUTINES) $(PG_NON_STANDARD) $(GR_ROUTINES) \
## 	   $(DISPATCH_ROUTINE) $(DRIVERS) $(SYSTEM_ROUTINES)
## 	ar ru libpgplot.a \
## 		`ls $(PG_ROUTINES) \
## 		$(PG_NON_STANDARD) $(GR_ROUTINES) $(DISPATCH_ROUTINE) \
## 		$(DRIVERS) $(SYSTEM_ROUTINES) | sort | uniq`
## 	$(RANLIB) libpgplot.a

add_definitions (-DPG_PPU)

set_source_files_properties (${pgplot_src}
  PROPERTIES COMPILE_FLAGS "-fPIC -O2 -fPIC"
  )

add_library (pgplot
  ${pgplot_src}
  ${pgplot_sys}
  ${pgplot_drivers}
  grexec.f
  )
target_link_libraries (pgplot
  ${pgplot_external_libraries}
  ${G2C_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${X11_LIBRARIES}
  ${ZLIB_LIBRARIES}
  )

## -- libXmPgplot --------------------------------

if (HAVE_XM)
  ## compiler instructions
  add_library (XmPgplot
    drivers/xmotif/XmPgplot.c
    )
  ## linker instructions
  add_dependencies (XmPgplot pgplot)
  target_link_libraries (XmPgplot pgplot ${XM_LIBRARIES})
  ## installation
  install (TARGETS XmPgplot
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (HAVE_XM)

## -- libtkpgplot --------------------------------

if (HAVE_TCL AND HAVE_TK)
  ## list of source files
  set (tkpgplot_sources drivers/xtk/tkpgplot.c)
  ## build the library
  add_library (tkpgplot ${tkpgplot_sources})
  target_link_libraries (tkpgplot pgplot)
  ## installation (needs to be conditional)
  install (TARGETS tkpgplot
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (HAVE_TCL AND HAVE_TK)

## ------------------------------------------------------------------------------
## Installation

find_path (pgplot_prefix release_area.txt
  PATHS
  ${pgplot_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (pgplot_prefix)
  message (STATUS "Installation area located for package pgplot.")
  set (CMAKE_INSTALL_PREFIX ${pgplot_prefix}) 
endif (pgplot_prefix)

## installation of libraries

install (TARGETS pgplot
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## installation of header files

set (pgplot_headers
  ${pgplot_SOURCE_DIR}/cpg/cpgplot.h
  ${pgplot_SOURCE_DIR}/drivers/commands.h
  ${pgplot_SOURCE_DIR}/drivers/defines.h
  ${pgplot_SOURCE_DIR}/drivers/pgxwin.h
  ${pgplot_SOURCE_DIR}/drivers/xathena/XaPgplot.h
  ${pgplot_SOURCE_DIR}/drivers/xathena/XaPgplotP.h
  ${pgplot_SOURCE_DIR}/drivers/xmotif/XmPgplot.h
  ${pgplot_SOURCE_DIR}/drivers/xmotif/XmPgplotP.h
  ${pgplot_SOURCE_DIR}/drivers/xtk/rvpgplot.h
  ${pgplot_SOURCE_DIR}/drivers/xtk/tkpgplot.h
  )

install (FILES ${pgplot_headers}
  DESTINATION include/pgplot
  )
