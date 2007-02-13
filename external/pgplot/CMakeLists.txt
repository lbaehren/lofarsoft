project (pgplot C Fortran)

subdirs (build)

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
