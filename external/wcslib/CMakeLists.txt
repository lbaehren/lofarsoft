## WCSLIB is a C library, supplied with a full set of Fortran wrappers, that
## implements the "World Coordinate System" (WCS) convention in FITS (Flexible
## Image Transport System).  It also includes a PGPLOT-based routine, PGSBOX,
## for drawing general curvilinear coordinate graticules.
##
## The FITS data format is widely used within the international astronomical
## community, from the radio to gamma-ray regimes, for data interchange and
## archive, and also increasingly as an online format.  It is described in
##
##   "Definition of The Flexible Image Transport System (FITS)",
##    Hanisch, R.J., Farris, A., Greisen, E.W., et al. 2001, A&A, 376, 359
##
## which formalizes NOST 100-2.0, a document produced by the NASA/Science
## Office of Standards and Technology, see http://fits.gsfc.nasa.gov.

project (WCSLIB C Fortran)

subdirs (
  C
  FORTRAN
  pgsbox
  )

## ------------------------------------------------------------------------------
## Find required external components

INCLUDE (../../devel_common/cmake/FindG2C.cmake)

## CFITSIO is required to compile getwcstab.c which contains an
##   implementation of fits_read_wcstab().  However, this function is
##   included in the latest releases of CFITSIO (post 3.004beta).  It is
##   required here only for test programs twcstab and twcshdr, and is not
##   inserted into the WCSLIB object library.
##
##   If available, CFITSIO is also optionally used for test programs
##   tfitshdr, tpih1 and tpih2 by setting preprocessor macro
##   -DDO_CFITSIO.

INCLUDE (../../devel_common/cmake/FindCFITSIO.cmake)

#if (HAVE_CFITSIO)
#  add_definitions (-DDO_CFITSIO)
#endif (HAVE_CFITSIO)

## PGPLOT is Tim Pearson's FORTRAN graphics library with separate C
##   interface available from astro.caltech.edu.  It is only required by
##   test programs that plot test grids (tprj2, tcel1, tcel2, tspc,
##   ttab2, ttab3, twcsmix, and tpih2).  You can skip these by setting
##   PGPLOTLIB to blank.  
##
##   It is difficult for configure to deduce what auxiliary graphics
##   libraries may be needed for PGPLOT since it depends on which of many
##   possible graphics drivers were selected when PGPLOT was installed.
##   Therefore it is quite likely that you will need to add additional
##   libraries to PGPLOTLIB.

INCLUDE (../../devel_common/cmake/FindPGPLOT.cmake)

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

file (GLOB libwcs_sources
  C/*.c
  FORTRAN/*.c
  )

add_library (wcs
  ${libwcs_sources}
  )

target_link_libraries (wcs ${CFITSIO_LIBRARIES})

## libpgsbox

file (GLOB pgsbox_sources pgsbox/*.f pgsbox/*.c)

add_library (pgsbox ${pgsbox_sources})

target_link_libraries (pgsbox wcs ${PGPLOT_LIBRARIES})

## ------------------------------------------------------------------------------
## The test programs for the WCSLIB

set (WCSLIB_TEST_PROGRAMS
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

## PGPLOTLIB := -lcpgplot -lpgplot -lpng -lz -lX11 -lg2c 

foreach (TEST_PROGRAM ${WCSLIB_TEST_PROGRAMS})
  add_executable (${TEST_PROGRAM} C/test/${TEST_PROGRAM}.c)
  target_link_libraries (${TEST_PROGRAM}
    pgsbox
    wcs
    ${PGPLOT_LIBRARIES}
    ${PNG_LIBRARIES}
    ${libz}
    ${MOTIF_LIBRARIES}
    ${G2C_LIBRARIES}
    )
endforeach (TEST_PROGRAM)
