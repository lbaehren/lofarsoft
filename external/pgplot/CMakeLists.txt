project (pgplot C Fortran)

subdirs (
  build
  drivers
  examples
  fonts
  src
  sys
  )

set (pgplot_modules
  cpg
  drivers
  drivers/xmotif
  src
  sys
)

## ------------------------------------------------------------------------------
## Required external packages

find_package (PNG)
find_package (X11)
find_package (ZLIB)

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

find_library (libgif gif
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -- Tcl/Tk -------------------------------------

find_path (tcl_inc tcl.h
  PATHS /usr/include /usr/local/include /sw/include
  PATH_SUFFIXES tcl tcl8.4
  NO_DEFAULT_PATH
  )

find_library (libtcl tcl
  PATHS /usr/lib /usr/local/lib /sw/lib
  NO_DEFAULT_PATH
  )

find_library (libtk tk
  PATHS /usr/lib /usr/local/lib /sw/lib
  NO_DEFAULT_PATH
  )

## summary of external libraries

message (STATUS "libg2c = ${libg2c}")
message (STATUS "libtcl = ${libtcl} , ${tcl_inc}")
message (STATUS "libtk  = ${libtk}")
message (STATUS "libx11 = ${X11_LIBRARIES}")

## ------------------------------------------------------------------------------
## Create list of drivers to be included into the library

set (outfile "build/drivers.list")

file (WRITE ${outfile} "! List of selected drivers\n\n")

file (APPEND ${outfile} "NUDRIV 0 /NULL     Null device	 Std F77\n")
file (APPEND ${outfile} "PGDRIV 0 /PGMF     PGPLOT metafile  Std F77\n")
file (APPEND ${outfile} "PSDRIV 1 /PS       PostScript printers, monochrome, landscape	Std F77\n")
file (APPEND ${outfile} "PSDRIV 2 /VPS      Postscript printers, monochrome, portrait		Std F77\n")
file (APPEND ${outfile} "PSDRIV 3 /CPS      PostScript printers, color, landscape		Std F77\n")
file (APPEND ${outfile} "PSDRIV 4 /VCPS     PostScript printers, color, portrait		Std F77\n")
file (APPEND ${outfile} "WDDRIV 1 /WD       X Window dump file, landscape\n")
file (APPEND ${outfile} "WDDRIV 2 /VWD      X Window dump file, portrait\n")
file (APPEND ${outfile} "XWDRIV 1 /XWINDOW  Workstations running X Window System		C\n")
file (APPEND ${outfile} "XWDRIV 2 /XSERVE   Persistent window on X Window System		C\n")
file (APPEND ${outfile} "XMDRIV 0 /XMOTIF   Motif applications containing XmPgplot widgets.   C\n")

if (libgif)
 file (APPEND ${outfile} "GIDRIV 1 /GIF      GIF-format file, landscape\n")
 file (APPEND ${outfile} "GIDRIV 2 /VGIF     GIF-format file, portrait\n")
endif (libgif)

if (PNG_LIBRARIES)
  message ("PNDRIV 1 /PNG      Portable Network Graphics file                 C\n")
  message ("PNDRIV 2 /TPNG     Portable Network Graphics file - transparent background C\n")
endif (PNG_LIBRARIES)

if (libtcl AND libtk)
  message ("TKDRIV 0 /XTK      X-window Tcl/Tk programs with pgplot widgets.  C\n")
endif (libtcl AND libtk)

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

include_directories (
  .
  ${pgplot_SOURCE_DIR}
  ${pgplot_modules}
  ${tcl_inc}
  ${PNG_INCLUDE_DIR}
  ${X11_INCLUDE_DIR}
)

FILE (GLOB pgplot_fonts ./fonts/*.f)
FILE (GLOB pgplot_src ./src/*.f ./src/*.c)
FILE (GLOB pgplot_sys ./sys/*.f ./sys/*.c)

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
  ./drivers/xmotif/XmPgplot.c
  ./drivers/xtk/tkpgplot.c
  )

## -- libpgplot ----------------------------------

add_library (pgplot
  ${pgplot_src}
  ${pgplot_sys}
  ${pgplot_drivers}
  grexec.f
  )
target_link_libraries (pgplot
  ${libg2c}
  ${libgif}
  ${libtcl}
  ${libtk}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
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

## -- libtkpgplot --------------------------------

add_library (tkpgplot
  drivers/xtk/tkpgplot.c
  )

## ------------------------------------------------------------------------------
## Demo programs
##
##  When linking, use
##    -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L%p/lib -lpng -lz
##    -laquaterm -L$PGPLOT_DIR/lib -lpgplot

set (examples
  pgdemo1
  pgdemo2
  pgdemo3
  pgdemo4
  pgdemo5
  pgdemo6
  )

foreach (src ${examples})
  add_executable (${src} examples/${src}.f)
  target_link_libraries (${src}
    pgplot
    XmPgplot
    tkpgplot
    ${libg2c}
    ${libgif}
    ${libtcl}
    ${libtk}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${X11_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )
  set_target_properties (${src}
    PROPERTIES LINKER_LANGUAGE Fortran)
endforeach (src)
