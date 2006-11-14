#-----------------------------------------------------------------------------
#   GNU makefile for building the WCSLIB 4.2 FORTRAN wrappers.
#
#   Summary of the main targets
#   ---------------------------
#      all:       Build the library
#      clean:     Delete intermediate object files.
#      cleaner:   clean, and also delete the test executables.
#      realclean (or cleanest): cleaner, and also delete the object library
#                 and configure-generated files, config.h and GNUmakefile.
#      check (or test): Compile and run the test programs.
#      tests:     Compile the test programs (but don't run them).
#
#   Notes:
#      1) If you need to make changes then preferably modify ../makedefs.in
#         instead and re-run configure.
#
#      2) This makefile assumes that the WCSLIB 4.2 sources reside in ../C
#         (as in the distribution kit).
#
#   Author: Mark Calabretta, Australia Telescope National Facility
#   http://www.atnf.csiro.au/~mcalabre/index.html
#   $Id: GNUmakefile,v 4.2 2005/09/23 04:25:55 cal103 Exp $
#-----------------------------------------------------------------------------
# Get configure settings.
include ../makedefs

LIBWCS  := ../C/$(LIBWCS)
MODULES := cel_f.o \
           fitshdr_f.o \
           lin_f.o \
           log_f.o \
           prj_f.o \
           spc_f.o \
           sph_f.o \
           spx_f.o \
           tab_f.o \
           wcs_f.o \
           wcsfix_f.o \
           wcshdr_f.o \
           wcsunits_f.o

# Test programs that don't require PGPLOT.
TEST_N := tlin tlog tprj1 tsph tspx ttab1 twcs twcssub tpih1 tfitshdr tunits \
          twcsfix

# Test programs that do require PGPLOT.
TEST_P := tspc tprj2 tcel1 ttab2 ttab3 twcsmix

TESTS  := $(TEST_N)
ifneq "$(PGPLOTINC)" ""
ifneq "$(PGPLOTLIB)" ""
  TESTS   += $(TEST_P) tpih2
endif
endif

# Test programs that require CFITSIO.
ifneq "$(CFITSIOINC)" ""
ifneq "$(CFITSIOLIB)" ""
TESTS  += twcstab
endif
endif

PGSBOXLIB := ../pgsbox/libpgsbox-$V.a
PGPLOTLIB := $(PGPLOTLIB)

vpath %.h ../C

# Pattern rules
#--------------

%.o : %.c
	-@ echo ''
	   $(CC) $(CFLAGS) -I../C -c $<

%.i : %.c
	-@ echo ''
	   $(CC) $(CFLAGS) -E -I../C -o $@ $<

%.d : %.c
	-@ echo ''
	-@ $(CC) $(CFLAGS) -E -I../C $< | \
	   sed -n -e 's|^# 1 "\([^/].*\.h\)".*|\1|p' | \
	   sort -u

%.o : %.f
	-@ echo ''
	   $(FC) $(FFLAGS) -c $<

run_% : %
	-@ echo ''
	-@ $(TIMER)
	 @ if [ "$<" = tunits ] ; then \
	     $< < ../C/test/units_test ; \
	   else \
	     $< < /dev/null 2>&1 ; \
	   fi
	-@ echo ''

# Static and static pattern rules
#--------------------------------

.PHONY : all check clean cleaner cleanest install lib realclean test tests

all : show_all lib

lib : $(LIBWCS)

$(LIBWCS) ::
	-@ echo ''
	   $(MAKE) -C ../C lib

$(LIBWCS) :: $(MODULES:%=$(LIBWCS)(%))
	-@ echo ''
	   $(RANLIB) $(LIBWCS)

getwcstab_f.o : getwcstab_f.c getwcstab.h
	-@ echo ''
	   $(CC) $(CFLAGS) -I../C $(CFITSIOINC) -c $<

../C/getwcstab.o ::
	-@ echo ''
	   $(MAKE) -C ../C getwcstab.o

install : lib
	$(MAKE) -C ../C install
	$(INSTALL) -m 444 CHANGES $(INCDIR)/CHANGES_FORTRAN
	$(INSTALL) -m 444 *.inc $(INCDIR)

clean :
	- $(RM) *.o *.i a.out core fort.* $(EXTRA_CLEAN)

cleaner : clean
	- $(RM) $(TEST_N)
	- $(RM) $(TEST_P) tpih2 twcstab
	- $(RM) tofits test.fits

cleanest realclean : cleaner
	- $(RM) $(LIBWCS)

check test : tests $(TESTS:%=run_%)

tests : $(TESTS)

$(TEST_N) : % : test/%.f $(LIBWCS)
	-@ echo ''
	   $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $< $(LIBWCS) $(LIBS)
	   $(RM) $@.o

$(TEST_P) : % : test/%.f $(LIBWCS)
	-@ echo ''
	   $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $< $(LIBWCS) $(PGPLOTLIB) $(LIBS)
	   $(RM) $@.o

tpih2 : % : test/%.f $(PGSBOXLIB) $(LIBWCS)
	-@ echo ''
	   $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $< $(PGSBOXLIB) $(LIBWCS) \
	     $(PGPLOTLIB) $(LIBS)
	   $(RM) $@.o

twcstab : test/twcstab.f ../C/wcstab.fits getwcstab_f.o getwcstab.inc \
          ../C/getwcstab.o $(LIBWCS)
	-@ echo ''
	   $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $< getwcstab_f.o \
	     ../C/getwcstab.o $(LIBWCS) $(CFITSIOLIB) $(LIBS)
	   $(RM) $@.o

$(PGSBOXLIB) :
	-@ echo ''
	   $(MAKE) -C ../pgsbox lib

tofits : ../C/test/tofits.c
	$(CC) $(CFLAGS) -o $@ $<

test.fits : ../C/test/wcs.cards tofits
	sed '/^BADCARD/q' $< | tofits > $@

GNUmakefile : ../makedefs ;

../makedefs : ../makedefs.in ../config.status
	cd .. && config.status

# Dependencies
#-------------

$(LIBWCS)(cel_f.o)      : cel.h prj.h
$(LIBWCS)(fitshdr_f.o)  : fitshdr.h wcsutil.h
$(LIBWCS)(lin_f.o)      : lin.h
$(LIBWCS)(log_f.o)      : log.h
$(LIBWCS)(prj_f.o)      : prj.h
$(LIBWCS)(spc_f.o)      : spc.h spx.h wcsutil.h
$(LIBWCS)(sph_f.o)      : sph.h
$(LIBWCS)(spx_f.o)      : spx.h
$(LIBWCS)(tab_f.o)      : tab.h
$(LIBWCS)(wcs_f.o)      : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcsutil.h
$(LIBWCS)(wcsfix_f.o)   : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcsfix.h
$(LIBWCS)(wcshdr_f.o)   : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h
$(LIBWCS)(wcsunits_f.o) : wcsunits.h wcsutil.h

tcel1   : cel.inc prj.inc
tfitshdr: fitshdr.inc test.fits
tlin    : lin.inc
tlog    : log.inc
tpih1   : wcs.inc wcshdr.inc test.fits
tpih2   : wcs.inc wcshdr.inc test.fits
tprj1   : prj.inc
tprj2   : prj.inc
tspc    : spc.inc spx.inc
tspx    : spx.inc
ttab1   : tab.inc
ttab2   : tab.inc
ttab3   : prj.inc tab.inc
tunits  : wcsunits.inc
twcs    : cel.inc prj.inc wcs.inc
twcsfix : wcs.inc
twcsmix : cel.inc lin.inc prj.inc wcs.inc
twcssub : cel.inc prj.inc wcs.inc
twcstab : wcs.inc wcshdr.inc
