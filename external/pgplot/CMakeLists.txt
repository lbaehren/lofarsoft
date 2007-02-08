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

include_directories (
  .
  ${pgplot_modules}
)

## ------------------------------------------------------------------------------
## Start building the library
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
  )
