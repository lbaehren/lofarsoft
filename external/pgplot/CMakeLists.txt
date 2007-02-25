project (pgplot C Fortran)

subdirs (build)

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

## -- Tcl/Tk -------------------------------------

find_path (tcl_inc tcl.h
  PATHS /usr/include /usr/local/include /sw/include
  PATH_SUFFIXES tcl tcl8.4
  NO_DEFAULT_PATH
  )

find_library (tcl_lib tcl
  PATHS /usr/lib /usr/local/lib /sw/lib
  NO_DEFAULT_PATH
  )

find_library (tk_lib tk
  PATHS /usr/lib /usr/local/lib /sw/lib
  NO_DEFAULT_PATH
  )

## summary of external libraries

message (STATUS "libg2c = ${libg2c}")
message (STATUS "libtcl = ${tcl_lib} , ${tcl_inc}")
message (STATUS "libtk  = ${tk_lib}")
message (STATUS "libx11 = ${X11_LIBRARIES}")

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
  ${tcl_lib}
  ${tk_lib}
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
    ${tcl_lib}
    ${tk_lib}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${X11_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )
  set_target_properties (${src}
    PROPERTIES LINKER_LANGUAGE Fortran)
endforeach (src)
