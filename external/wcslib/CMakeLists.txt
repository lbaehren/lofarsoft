PROJECT (WCSLIB)

## ------------------------------------------------------------------------------
## Find required external components

INCLUDE (../../devel_common/cmake/FindCFITSIO.cmake)
INCLUDE (../../devel_common/cmake/FindPGPLOT.cmake)
INCLUDE (FindPNG)
INCLUDE (FindMotif)

FIND_LIBRARY (G2C_LIBRARY g2c /usr/lib /usr/local/lib)

## ------------------------------------------------------------------------------
## Set paths where to look for header files

INCLUDE_DIRECTORIES (
		    .
		    ..
		    ${WCSLIB_SOURCE_DIR}/C
		    ${WCSLIB_SOURCE_DIR}/C/test
		    ${WCSLIB_SOURCE_DIR}/pgsbox
		    ${WCSLIB_SOURCE_DIR}/FORTRAN
		    ${CFITSIO_INCLUDE_DIR}
		    ${PPGPLOT_INCLUDE_DIR}
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

SET (WCSLIB_FORTRAN_SOURCES
    FORTRAN/cel_f.c
    FORTRAN/fitshdr_f.c
    FORTRAN/getwcstab_f.c
    FORTRAN/lin_f.c
    FORTRAN/log_f.c
    FORTRAN/prj_f.c
    FORTRAN/spc_f.c
    FORTRAN/sph_f.c
    FORTRAN/spx_f.c
    FORTRAN/tab_f.c
    FORTRAN/wcs_f.c
    FORTRAN/wcsfix_f.c
    FORTRAN/wcshdr_f.c
    FORTRAN/wcsunits_f.c
)

ADD_LIBRARY (wcs
    C/cel.c
    C/fitshdr.c
    C/getwcstab.c
    C/lin.c
    C/log.c
    C/prj.c
    C/spc.c
    C/sph.c
    C/spx.c
    C/tab.c
    C/wcs.c
    C/wcsfix.c
    C/wcshdr.c
    C/wcspih.c
    C/wcstrig.c
    C/wcsulex.c
    C/wcsunits.c
    C/wcsutil.c
    C/wcsutrn.c
)

ADD_LIBRARY (pgsbox
    pgsbox/cpgsbox.c
#    pgsbox/cpgtest.c
    pgsbox/pgwcsl.c
    C/cel.c
    C/lin.c
    C/prj.c
    C/spc.c
    C/sph.c
    C/spx.c
    C/tab.c
    C/wcs.c
    ${WCSLIB_FORTRAN_SOURCES}
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
	TARGET_LINK_LIBRARIES (${TEST_PROGRAM} pgsbox wcs XmPgplot cpgplot pgplot ${MOTIF_LIBRARIES})
ENDFOREACH (TEST_PROGRAM)
