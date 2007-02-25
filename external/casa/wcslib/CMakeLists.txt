PROJECT (WCSLIB)

## ------------------------------------------------------------------------------
## Find required external components
##
## AC_CHECK_LIB([z],       [deflate],   [PGPLOTLIB="-lz $PGPLOTLIB"],
##              [], [$PGPLOTLIB $LIBS])
## AC_CHECK_LIB([png],     [png_error], [PGPLOTLIB="-lpng $PGPLOTLIB"],
##              [], [$PGPLOTLIB $LIBS])
## AC_CHECK_LIB([pgplot],  [pgbeg_],    [PGPLOTLIB="-lpgplot $PGPLOTLIB"],
##              [], [$PGPLOTLIB $LIBS])
## AC_CHECK_LIB([cpgplot], [cpgbeg],    [PGPLOTLIB="-lcpgplot $PGPLOTLIB"],
##              [PGPLOTLIB=], [$PGPLOTLIB $LIBS])

INCLUDE (../../../devel_common/cmake/FindG2C.cmake)
INCLUDE (../../../devel_common/cmake/FindPGPLOT.cmake)

find_package (PNG)
find_package (Motif)

FIND_LIBRARY (libz z
  /usr/lib /usr/local/lib
  )

## ------------------------------------------------------------------------------
## Set paths where to look for header files

INCLUDE_DIRECTORIES (
  .
  ..
  ${WCSLIB_SOURCE_DIR}/C
  ${WCSLIB_SOURCE_DIR}/C/test
  ${WCSLIB_SOURCE_DIR}/pgsbox
  ${WCSLIB_SOURCE_DIR}/FORTRAN
  ${CFITSIO_INCLUDES}
  ${PGPLOT_INCLUDES}
  )

## ------------------------------------------------------------------------------
## Build the WCSLIB
##
## The original GNUMakefile provided with the source code states:
##
##  $(LIBWCS) : $(MODULES:%=$(LIBWCS)(%))
##	-@ echo ''
##	   $(RANLIB) $@
##
##  lib : fitshdr.c wcspih.c wcsulex.c wcsutrn.c $(LIBWCS)
##

FILE (GLOB libwcs_sources C/*.c FORTRAN/*.c)

ADD_LIBRARY (wcs
  ${libwcs_sources}
  )

## ------------------------------------------------------------------------------
## The test programs for the WCSLIB

SET (WCSLIB_TEST_PROGRAMS
  tcel1
  tcel2
  tfitshdr
  tlin
  tlog
  tofits
  tpih1
  tpih2
  tprj1
  tprj2
  tspc
  tsph
  tspx
  ttab1
  ttab2
  ttab3
  tunits
  twcs
  twcsfix
  twcshdr
  twcsmix
  twcssub
  twcstab
  )

FOREACH (TEST_PROGRAM ${WCSLIB_TEST_PROGRAMS})
  ADD_EXECUTABLE (${TEST_PROGRAM} C/test/${TEST_PROGRAM}.c)
  TARGET_LINK_LIBRARIES (${TEST_PROGRAM}
    pgsbox
    wcs
    ${PGPLOT_LIBRARIES}
    ${libz}
    ${MOTIF_LIBRARIES})
ENDFOREACH (TEST_PROGRAM)
