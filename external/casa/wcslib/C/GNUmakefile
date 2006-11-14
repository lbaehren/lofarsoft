#-----------------------------------------------------------------------------
#   GNU makefile for building WCSLIB 4.2.
#
#   Summary of the main targets
#   ---------------------------
#      all:       Build the library.
#      clean:     Delete intermediate object files.
#      cleaner:   clean, and also delete the test executables.
#      realclean (or cleanest): cleaner, and also delete the object library
#                 and configure-generated files, config.h and GNUmakefile.
#      check (or test): Compile and run the test programs.
#      tests:     Compile the test programs (but don't run them).
#      check_s:   Compile and run the scalar test programs.
#      tests_s:   Compile the scalar test programs (but don't run them).
#
#   Notes:
#      1) If you need to make changes then preferably modify ../makedefs.in
#         instead and re-run configure.
#
#   Author: Mark Calabretta, Australia Telescope National Facility
#   http://www.atnf.csiro.au/~mcalabre/index.html
#   $Id: GNUmakefile,v 4.2 2005/09/23 04:25:05 cal103 Exp $
#-----------------------------------------------------------------------------
# Get configure settings.
include ../makedefs

VPATH := .:./test

MODULES := cel.o \
           fitshdr.o \
           lin.o \
           log.o \
           prj.o \
           spc.o \
           sph.o \
           spx.o \
           tab.o \
           wcs.o \
           wcsfix.o \
           wcshdr.o \
           wcspih.o \
           wcsulex.o  \
           wcsunits.o \
           wcsutil.o \
           wcsutrn.o

ifeq "$(WCSTRIG)" "MACRO"
  CPPFLAGS += -DWCSTRIG_MACRO
else
  ifneq "$(WCSTRIG)" "NATIVE"
    MODULES += wcstrig.o
  endif
endif

# Signals tfitshdr to use wcshdr().
ifneq "$(DO_WCSHDR)" ""
  CPPFLAGS += -DDO_WCSHDR
endif

# Test programs that don't require PGPLOT.
TEST_N := tlin tlog tprj1 tsph tspx ttab1 twcs twcssub tpih1 tfitshdr tunits \
          twcsfix

# Test programs that do require PGPLOT.
TEST_P := tspc tprj2 tcel1 tcel2 ttab2 ttab3 twcsmix

TESTS  := $(TEST_N)
ifneq "$(PGPLOTINC)" ""
ifneq "$(PGPLOTLIB)" ""
  TESTS   += $(TEST_P) tpih2
  TPRJ2_S := tprj2_s
endif
endif

# Test programs that require CFITSIO.
ifneq "$(CFITSIOINC)" ""
ifneq "$(CFITSIOLIB)" ""
  TESTS  += twcstab twcshdr
endif
endif


PGSBOXLIB := ../pgsbox/libpgsbox-$V.a

# Pattern rules
#--------------

%.c : %.l
	-@ echo ''
	   $(FLEX) $(FLFLAGS) -P$* $<
	   $(RM) $@
	   mv -f lex.$*.c $@

%.o : %.c
	-@ echo ''
	   $(CC) $(CPPFLAGS) $(CFLAGS) -c $<

%.i : %.c
	-@ echo ''
	   $(CC) $(CPPFLAGS) $(CFLAGS) -E -o $@ $<

%.d : %.c
	-@ echo ''
	-@ $(CC) $(CPPFLAGS) $(CFLAGS) -E $< | \
	   sed -n -e 's|^# 1 "\([^/].*\.h\)".*|\1|p' | \
	   sort -u

run_% : %
	-@ echo ''
	-@ $(TIMER)
	 @ if [ '$<' = tcel2 ] ; then \
	     echo N | $< ; \
	   elif [ '$<' = tunits ] ; then \
	     $< < test/units_test ; \
	   elif [ '$<' != twcshdr ] ; then \
	     $< < /dev/null 2>&1 ; \
	   fi
	-@ echo ''

# Static and static pattern rules
#--------------------------------

.PHONY : all check check_s clean cleaner cleanest install lib realclean \
         run_% test test_s tests tests_s

all : show_all lib

lib : fitshdr.c wcspih.c wcsulex.c wcsutrn.c $(LIBWCS)

$(LIBWCS) : $(MODULES:%=$(LIBWCS)(%))
	-@ echo ''
	   $(RANLIB) $@

getwcstab.o : getwcstab.c getwcstab.h
	-@ echo ''
	   $(CC) $(CPPFLAGS) $(CFLAGS) $(CFITSIOINC) -c $<

install : lib
	$(INSTALL) -d -m 2755 $(LIBDIR)
	$(INSTALL) -m 644 $(LIBWCS) $(LIBDIR)
	$(RM) $(LIBDIR)/libwcs.a
	$(LN_S) $(LIBDIR)/$(LIBWCS) $(LIBDIR)/libwcs.a
	$(INSTALL) -d -m 2755 $(INCDIR)
	$(INSTALL) -m 444 CHANGES $(INCDIR)/CHANGES_C
	$(INSTALL) -m 444 *.h $(INCDIR)
	$(RM) $(INCLINK)
	$(LN_S) $(INCDIR) $(INCLINK)

clean :
	- $(RM) *.o *.i a.out core $(EXTRA_CLEAN)

cleaner : clean
	- $(RM) $(TEST_N)
	- $(RM) $(TEST_P) tpih2 twcstab twcshdr
	- $(RM) tofits test.fits wcstab.fits
	- $(RM) t*_cfitsio
	- $(RM) tlin_s tprj1_s tprj2_s tsph_s

cleanest realclean : cleaner
	- $(RM) $(LIBWCS) config.h

check : tests $(TESTS:%=run_%)

tests : $(TESTS)

$(TEST_N) : % : test/%.c $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBWCS) $(LIBS)
	   $(RM) $@.o

$(TEST_P) : % : test/%.c $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(PGPLOTINC) $(CFLAGS) -c -o $@.o $<
	   $(LD) $(LDFLAGS) -o $@ $@.o $(PGPLOTLIB) $(LIBWCS) $(LIBS)
	   $(RM) $@.o

tpih2 : test/tpih2.c $(PGSBOXLIB) $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. -I../pgsbox $(PGPLOTINC) $(CFLAGS) -c \
	      -o $@.o $<
	   $(LD) $(LDFLAGS) -o $@ $@.o $(PGSBOXLIB) $(LIBWCS) $(PGPLOTLIB) $(LIBS)
	   $(RM) $@.o

tfitshdr_cfitsio tpih1_cfitsio : %_cfitsio : test/%.c $(LIBWCS)
	-@ echo ''
	   $(CC) -DDO_CFITSIO $(CPPFLAGS) -I. $(CFITSIOINC) $(CFLAGS) \
	      $(LDFLAGS) -o $@ $< $(CFITSIOLIB) $(LIBWCS) $(LIBS)
	   $(RM) $@.o

tpih2_cfitsio : test/tpih2.c $(PGSBOXLIB) $(LIBWCS)
	-@ echo ''
	   $(CC) -DDO_CFITSIO $(CPPFLAGS) -I. $(CFITSIOINC) $(PGPLOTINC) \
	      $(CFLAGS) -I../pgsbox -c -o $@.o $<
	   $(LD) $(LDFLAGS) -o $@ $@.o $(PGSBOXLIB) $(LIBWCS) $(PGPLOTLIB) \
	      $(CFITSIOLIB) $(LIBS)
	   $(RM) $@.o

twcstab : test/twcstab.c test/wcstab.cards getwcstab.o $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(CFITSIOINC) $(CFLAGS) \
	      $(LDFLAGS) -o $@ $< getwcstab.o $(LIBWCS) $(CFITSIOLIB) $(LIBS)
	   $(RM) $@.o

twcshdr : test/twcshdr.c getwcstab.o $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(CFITSIOINC) $(CFLAGS) \
	      $(LDFLAGS) -o $@ $< getwcstab.o $(LIBWCS) $(CFITSIOLIB) $(LIBS)
	   $(RM) $@.o

$(PGSBOXLIB) :
	-@ echo ''
	   $(MAKE) -C ../pgsbox lib

tofits : test/tofits.c
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -o $@ $<

test.fits : test/wcs.cards tofits
	sed '/^BADCARD/q' $< | tofits > $@

check_s test_s : tests_s run_tlin_s run_tprj1_s $(TPRJ2_S:%=run_%) run_tsph_s ;

tests_s : tlin_s tprj1_s $(TPRJ2_S) tsph_s ;

tlin_s tprj1_s tsph_s : % : test_s/%.c $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBWCS) $(LIBS)
	   $(RM) $@.o

tprj2_s : % : test_s/%.c $(LIBWCS)
	-@ echo ''
	   $(CC) $(CPPFLAGS) -I. $(PGPLOTINC) $(CFLAGS) -c -o $@.o $<
	   $(LD) $(LDFLAGS) -o $@ $@.o $(PGPLOTLIB) $(LIBWCS) $(LIBS)
	   $(RM) $@.o

GNUmakefile : ../makedefs ;

../makedefs config.h : config.h.in ../makedefs.in ../config.status
	cd .. && config.status

# Dependencies
#-------------

$(LIBWCS)(cel.o)      : cel.h prj.h sph.h wcsmath.h wcstrig.h
$(LIBWCS)(fitshdr.o)  : config.h fitshdr.h
$(LIBWCS)(lin.o)      : lin.h
$(LIBWCS)(log.o)      : log.h
$(LIBWCS)(prj.o)      : prj.h wcsmath.h wcstrig.h
$(LIBWCS)(spc.o)      : spc.h spx.h wcsmath.h wcstrig.h
$(LIBWCS)(sph.o)      : sph.h wcstrig.h
$(LIBWCS)(spx.o)      : spx.h
$(LIBWCS)(tab.o)      : tab.h wcsmath.h
$(LIBWCS)(wcs.o)      : cel.h lin.h log.h prj.h spc.h sph.h spx.h tab.h \
                        wcs.h wcsmath.h wcstrig.h wcsunits.h wcsutil.h
$(LIBWCS)(wcsfix.o)   : cel.h lin.h prj.h spc.h sph.h spx.h tab.h wcs.h \
                        wcsfix.h wcsmath.h wcsunits.h
$(LIBWCS)(wcshdr.o)   : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h \
                        wcsutil.h
$(LIBWCS)(wcspih.o)   : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h \
                        wcsmath.h
$(LIBWCS)(wcstrig.o)  : wcsmath.h wcstrig.h
$(LIBWCS)(wcsulex.o)  : wcsmath.h wcsunits.h
$(LIBWCS)(wcsunits.o) : wcsunits.h
$(LIBWCS)(wcsutil.o)  : wcsutil.h
$(LIBWCS)(wcsutrn.o)  : wcsunits.h

tcel1   : cel.h prj.h
tcel2   : cel.h prj.h
tfitshdr tfitshdr_cfitsio : fitshdr.h test.fits
tlin    : lin.h
tlin_s  : lin.h
tlog    : log.h
tpih1 tpih1_cfitsio : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcsfix.h \
                      wcshdr.h test.fits
tpih2 tpih2_cfitsio : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h \
                      test.fits
tprj1   : prj.h wcstrig.h
tprj1_s : prj.h wcstrig.h
tprj2   : prj.h
tprj2_s : prj.h
tspc    : spc.h spx.h wcstrig.h
tsph    : sph.h wcstrig.h
tsph_s  : sph.h wcstrig.h
tspx    : spx.h
ttab1   : tab.h
ttab2   : tab.h
ttab3   : prj.h tab.h
tunits  : wcsunits.h
twcs    : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h
twcsfix : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcsfix.h wcsunits.h
twcshdr : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h
twcsmix : cel.h lin.h prj.h spc.h sph.h spx.h tab.h wcs.h
twcssub : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h
twcstab : cel.h lin.h prj.h spc.h spx.h tab.h wcs.h wcshdr.h
