project (pgplot C Fortran)

set (pgplot_modules
  src
  sys
)

## ------------------------------------------------------------------------------
## Required external packages

find_package (PNG)
find_package (TCL)
find_package (X11)
find_package (ZLIB)

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

include_directories (
  .
  ${pgplot_modules}
  ${PNG_INCLUDE_DIR}
  ${TCL_INCLUDE_PATH}
  ${TK_INCLUDE_PATH}
  ${X11_INCLUDE_DIR}
)

message (STATUS "PNG includes : ${PNG_INCLUDE_DIR}")
message (STATUS "TCL inlcudes : ${TCL_INCLUDE_PATH}")
message (STATUS "TK  inlcudes : ${TK_INCLUDE_PATH}")
message (STATUS "X11 includes : ${X11_INCLUDE_DIR}")

## ------------------------------------------------------------------------------
## Create the drivers list
##
## In order to adhere to the basic installation instruction the list of selected
## drivers is created inside a separate directory, "./build", in which also the 
## later build will be carried out.

set (drivers "build/drivers.list")

FILE (MAKE_DIRECTORY "build")

file (WRITE ${drivers} "! List of selected drivers\n\n")

## GIF file format

file (APPEND ${drivers} " GIDRIV 1 /GIF      GIF-format file, landscape\n")
file (APPEND ${drivers} " GIDRIV 2 /VGIF     GIF-format file, portrait\n")

## Null device

file (APPEND ${drivers} " NUDRIV 0 /NULL     Null device	 Std F77\n")

## PGPLOT metafile

file (APPEND ${drivers} " PGDRIV 0 /PGMF     PGPLOT metafile  Std F77\n")

## PNG (Portable Network Graphics) -- optional

if (PNG_LIBRARIES)
  file (APPEND ${drivers} " PNDRIV 1 /PNG      Portable Network Graphics file                    C\n")
  file (APPEND ${drivers} " PNDRIV 2 /TPNG     Portable Network Graphics file - transparent background C\n")
endif (PNG_LIBRARIES)

## Postscript file formats

file (APPEND ${drivers} " PSDRIV 1 /PS       PostScript printers, monochrome, landscape	Std F77\n")
file (APPEND ${drivers} " PSDRIV 2 /VPS      Postscript printers, monochrome, portrait		Std F77\n")
file (APPEND ${drivers} " PSDRIV 3 /CPS      PostScript printers, color, landscape		Std F77\n")
file (APPEND ${drivers} " PSDRIV 4 /VCPS     PostScript printers, color, portrait		Std F77\n")

## Tcl/Tk

if (TCL_LIBRARY AND TK_LIBRARY)
 file (APPEND ${drivers} " TKDRIV 0 /XTK      X-window Tcl/Tk programs with pgplot widgets.     C\n")
endif (TCL_LIBRARY AND TK_LIBRARY)

## X-Windows

file (APPEND ${drivers} " WDDRIV 1 /WD       X Window dump file, landscape\n")
file (APPEND ${drivers} " WDDRIV 2 /VWD      X Window dump file, portrait\n")
file (APPEND ${drivers} " XWDRIV 1 /XWINDOW  Workstations running X Window System		C\n")
file (APPEND ${drivers} " XWDRIV 2 /XSERVE   Persistent window on X Window System		C\n")

## XMOTIF driver

file (APPEND ${drivers} " XMDRIV 0 /XMOTIF   Motif applications containing XmPgplot widgets.   C\n")

## ------------------------------------------------------------------------------
## Configure the build system

## ------------------------------------------------------------------------------
## Start building the library
## 
## Required PGPLOT sub-directories.
##   - drivers
##   - examples
##   - fonts
##   - src
##   - sys
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

FILE (GLOB pgplot_sysfonts_sources ./fonts/*.f)
FILE (GLOB pgplot_src_sources ./src/*.f ./src/*.c)
FILE (GLOB pgplot_sys_sources ./sys/*.f ./sys/*.c)

set (pgplot_drivers
  ./drivers/gidriv.f
  ./drivers/nudriv.f
  ./drivers/pgdriv.f
  ./drivers/pgxwin.c
  ./drivers/psdriv.f
  ./drivers/tkdriv.c
  ./drivers/wddriv.f
  ./drivers/xmdriv.c
  ./drivers/xwdriv.c
  )

add_library (pgplot
  ${pgplot_src_sources}
  ${pgplot_sys_sources}
  ${pgplot_drivers}
  grexec.f
  )
target_link_libraries (pgplot
  ${JPEG_LIBRARIES}
  ${X11_LIBRARIES}
  ${libg2c}
  )

## ------------------------------------------------------------------------------
## Demo programs
##
##  When linking, use
##    -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L%p/lib -lpng -lz
##    -laquaterm -L$PGPLOT_DIR/lib -lpgplot

add_executable (pgdemo1 examples/pgdemo1.f)
target_link_libraries (pgdemo1
  pgplot
  ${JPEG_LIBRARIES}
  ${X11_LIBRARIES}
  ${libg2c}
  )
