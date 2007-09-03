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

find_package (PNG)
find_package (X11)
find_package (ZLIB)

## check where to find the CMake scripts

find_path (pgplot_cmake FindBlitz.cmake FindCASA.cmake FindCFITSIO.cmake
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
endif (pgplot_cmake)

## Tcl -------------------------------------------

find_path (TCL_INCLUDES tcl.h
  PATHS ${include_locations}
  PATH_SUFFIXES tcl tcl8.4
  NO_DEFAULT_PATH
  )

find_library (TCL_LIBRARIES tcl
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (TCL_INCLUDES AND TCL_LIBRARIES)
  set (HAVE_TCL 1)
endif (TCL_INCLUDES AND TCL_LIBRARIES)

## Tk --------------------------------------------

find_path (TK_INCLUDES tk.h
  PATHS ${include_locations}
  PATH_SUFFIXES tcl tcl8.4 tk tk8.4
  NO_DEFAULT_PATH
  )

find_library (TK_LIBRARIES tk
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (TK_INCLUDES AND TK_LIBRARIES)
  set (HAVE_TK 1)
endif (TK_INCLUDES AND TK_LIBRARIES)

## X11 -------------------------------------------

find_path (Xm_INCLUDES XmP.h
  PATHS ${include_locations}
  PATH_SUFFIXES Xm
  NO_DEFAULT_PATH
  )

string (REGEX REPLACE "include/Xm" "include" Xm_INCLUDES ${Xm_INCLUDES})

find_library (libXaw Xaw PATHS ${lib_locations})
find_library (libXt Xt PATHS ${lib_locations})

## summary of external libraries

message (STATUS "libg2c        = ${G2C_LIBRARIES}")
message (STATUS "libXaw        = ${libXaw}")
message (STATUS "libXt         = ${libXt}")
message (STATUS "libx11        = ${X11_LIBRARIES}")
message (STATUS "TCL_INCLUDES  = ${TCL_INCLUDES}")
message (STATUS "TCL_LIBRARIES = ${TCL_LIBRARIES}")
message (STATUS "TK_INCLUDES   = ${TK_INCLUDES}")
message (STATUS "TK_LIBRARIES  = ${TK_LIBRARIES}")
message (STATUS "GIF_LIBRARIES = ${GIF_LIBRARIES}")
message (STATUS "PNG_LIBRARIES = ${PNG_LIBRARIES}")

## ------------------------------------------------------------------------------
## Create list of drivers to be included into the library

set (pgplot_external_libraries "")

set (outfile "build/drivers.list")

file (WRITE ${outfile} "! List of selected drivers\n\n")

file (APPEND ${outfile} "NUDRIV 0 /NULL     Null device	 Std F77\n")
file (APPEND ${outfile} "PGDRIV 0 /PGMF     PGPLOT metafile  Std F77\n")
file (APPEND ${outfile} "PSDRIV 1 /PS       PostScript printers, monochrome, landscape	Std F77\n")
file (APPEND ${outfile} "PSDRIV 2 /VPS      Postscript printers, monochrome, portrait		Std F77\n")
file (APPEND ${outfile} "PSDRIV 3 /CPS      PostScript printers, color, landscape		Std F77\n")
file (APPEND ${outfile} "PSDRIV 4 /VCPS     PostScript printers, color, portrait		Std F77\n")
file (APPEND ${outfile} "RVDRIV 0 /XRV       X-window Rivet/Tk programs with pgplot widgets.   C")
file (APPEND ${outfile} "WDDRIV 1 /WD       X Window dump file, landscape\n")
file (APPEND ${outfile} "WDDRIV 2 /VWD      X Window dump file, portrait\n")
file (APPEND ${outfile} "XMDRIV 0 /XMOTIF   Motif applications containing XmPgplot widgets.   C\n")

if (libXaw AND libXt)
  file (APPEND ${outfile} "XWDRIV 1 /XWINDOW  Workstations running X Window System		C\n")
  file (APPEND ${outfile} "XWDRIV 2 /XSERVE   Persistent window on X Window System		C\n")
  ## update linker list
  list (APPEND pgplot_external_libraries ${libXaw})
  list (APPEND pgplot_external_libraries ${libXt})
endif (libXaw AND libXt)

if (GIF_LIBRARIES)
 file (APPEND ${outfile} "GIDRIV 1 /GIF      GIF-format file, landscape\n")
 file (APPEND ${outfile} "GIDRIV 2 /VGIF     GIF-format file, portrait\n")
 ## update linker list
 list (APPEND pgplot_external_libraries ${GIF_LIBRARIES})
endif (GIF_LIBRARIES)

if (PNG_LIBRARIES)
 file (APPEND ${outfile} "PNDRIV 1 /PNG      Portable Network Graphics file                 C\n")
 file (APPEND ${outfile} "PNDRIV 2 /TPNG     Portable Network Graphics file - transparent background C\n")
 ## update linker list
 list (APPEND pgplot_external_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (TCL_LIBRARIES AND TK_LIBRARIES)
  file (APPEND ${outfile} "TKDRIV 0 /XTK      X-window Tcl/Tk programs with pgplot widgets.  C\n")
  ## update linker list
  list (APPEND pgplot_external_libraries ${TCL_LIBRARIES})
  list (APPEND pgplot_external_libraries ${TK_LIBRARIES})
endif (TCL_LIBRARIES AND TK_LIBRARIES)

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
## .
## |-- applications
## |   |-- curvefit
## |   |-- plot10
## |   `-- plotpg
## |-- cpg
## |-- drivers
## |   |-- old
## |   |-- xathena
## |   |-- xmotif
## |   `-- xtk
## |-- examples
## |-- fonts
## |-- pgdispd
## |-- pgmf
## |-- src
## |-- sys
## 
## DEFAULT_TARGETS="lib grfont.dat prog pgplot.doc"
##
## lib : libpgplot.a $(SHARED_LIB)
## 
## libpgplot.a : $(PG_ROUTINES) $(PG_NON_STANDARD) $(GR_ROUTINES) \
## 	   $(DISPATCH_ROUTINE) $(DRIVERS) $(SYSTEM_ROUTINES)
## 	ar ru libpgplot.a \
## 		`ls $(PG_ROUTINES) \
## 		$(PG_NON_STANDARD) $(GR_ROUTINES) $(DISPATCH_ROUTINE) \
## 		$(DRIVERS) $(SYSTEM_ROUTINES) | sort | uniq`
## 	$(RANLIB) libpgplot.a
##

add_subdirectory (drivers)
add_subdirectory (fonts)
add_subdirectory (src)
add_subdirectory (sys)

set (pgplot_modules
  cpg
  drivers
  drivers/xmotif
  src
  sys
)

include_directories (
  .
  ${pgplot_SOURCE_DIR}
  ${pgplot_modules}
  ${TCL_INCLUDES}
  ${PNG_INCLUDE_DIR}
  ${X11_INCLUDE_DIR}
  ${Xm_INCLUDES}
)

FILE (GLOB pgplot_src ./src/*.f ./src/*.c)
FILE (GLOB pgplot_sys ./sys/*.f ./sys/*.c)
FILE (GLOB pgplot_fonts ./fonts/*.f)

set (pgplot_drivers
  ./drivers/gidriv.f
  ./drivers/nudriv.f
  ./drivers/pgdriv.f
  ./drivers/pgxwin.c
  ./drivers/psdriv.f
  ./drivers/rvdriv.c
  ./drivers/tkdriv.c
  ./drivers/wddriv.f
  ./drivers/xmdriv.c
  ./drivers/xwdriv.c
  ./drivers/xmotif/XmPgplot.c
  ./drivers/xtk/tkpgplot.c
  )

## -- libpgplot ----------------------------------

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

## -- libcpgplot ---------------------------------

add_library (cpgplot
  cpg/cpgdemo.c
  cpg/pgbind.c
  )

## -- libXmPgplot --------------------------------

add_library (XmPgplot
  drivers/xmotif/XmPgplot.c
  )
target_link_libraries (XmPgplot pgplot)

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

install (TARGETS pgplot cpgplot XmPgplot
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
