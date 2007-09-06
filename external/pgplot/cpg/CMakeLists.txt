
## -----------------------------------------------------------------------------
## Library (cpgplot)
##
## libcpgplot.a cpgplot.h: $(PG_SOURCE) pgbind 
## 	./pgbind $(PGBIND_FLAGS) -h -w $(PG_SOURCE)
## 	$(CCOMPL) -c $(CFLAGC) cpg*.c
## 	rm -f cpg*.c
## 	ar ru libcpgplot.a cpg*.o
## 	$(RANLIB) libcpgplot.a
## 	rm -f cpg*.o

file (GLOB cpgplot_sources
  ${pgplot_SOURCE_DIR}/src/*.c
  ${pgplot_SOURCE_DIR}/src/*.f
  ${pgplot_SOURCE_DIR}/sys/*.c
  ${pgplot_SOURCE_DIR}/cpg/*.c
  )

add_library (cpgplot ${cpgplot_sources})
add_dependencies (cpgplot pgplot)
target_link_libraries (cpgplot pgplot)

## -----------------------------------------------------------------------------
## Demo program
##
## cpgdemo: cpgplot.h $(SRC)/cpg/cpgdemo.c libcpgplot.a
##	$(CCOMPL) $(CFLAGD) -c -I. $(SRC)/cpg/cpgdemo.c
##	$(FCOMPL) -o cpgdemo cpgdemo.o $(CPGPLOT_LIB) $(LIBS)
##	rm -f cpgdemo.o
##

## compiler instructions

add_executable (cpgdemo cpgdemo.c)

## linker instructions

target_link_libraries (cpgdemo cpgplot)

## -----------------------------------------------------------------------------
## installation

install (TARGETS cpgplot cpgdemo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

