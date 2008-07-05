#-----------------------------------------------------------------------------
#   GNU makefile for building PGSBOX 4.2.
#
#   Summary of the main targets
#   ---------------------------
#      all:       Build the library.
#      clean:     Delete intermediate object files.
#      cleaner:   clean, and also delete the test executables.
#      realclean (or cleanest):  cleaner, and also delete the object library,
#                 the WCSLIB object library, and GNUmakefile.
#      check (or test): Compile and run the test programs.
#      tests:     Compile the test programs (but don't run them).
#
#   Notes:
#      1) If you need to make changes then preferably modify ../makedefs.in
#         instead and re-run configure.
#
#      2) In compiling the test programs, this makefile assumes that the
#         WCSLIB 4.2 sources reside in ../C and ../FORTRAN (as in the
#         distribution kit).
#
#   Author: Mark Calabretta, Australia Telescope National Facility
#   http://www.atnf.csiro.au/~mcalabre/index.html
#   $Id: GNUmakefile,v 4.2 2005/09/23 04:26:18 cal103 Exp $
#-----------------------------------------------------------------------------
# Get configure settings.
PIC := TRUE
include ../makedefs

LIBWCS := ../C/$(LIBWCS)

vpath %.h   ../C
vpath %.inc ../FORTRAN

PGSBOXLIB := libpgsbox-$V.a
MODULES := pgsbox.o \
           cpgsbox.o \
           pgwcsl.o \
           pgcrfn.o

# Pattern rules
#--------------

%.o : %.c
	-@ echo ''
	   $(CC) $(CPPFLAGS) $(CFLAGS) -c -I. -I../C $<

%.i : %.c
	-@ echo ''
	   $(CC) $(CPPFLAGS) $(CFLAGS) -E -I. -I../C -o $@ $<

%.d : %.c
	-@ echo ''
	-@ $(CC) $(CPPFLAGS) $(CFLAGS) -E -I. -I../C $< | \
	   sed -n -e 's|^# 1 "\([^/].*\.h\)".*|\1|p' | \
	   sort -u

%.o : %.F
	-@ echo ''
	   $(FC) $(FFLAGS) -I../FORTRAN -c -o $@ $<

%.o : %.f
	-@ echo ''
	   $(FC) $(FFLAGS) -I../FORTRAN -c -o $@ $<

# Static and static pattern rules
#--------------------------------

.PHONY : all check clean cleaner cleanest install realclean test tests wcslib

all : show_all lib

lib : $(PGSBOXLIB)

$(PGSBOXLIB) : $(MODULES:%=$(PGSBOXLIB)(%))
	-@ echo ''
	   $(RANLIB) $@

clean :
	- $(RM) *.o *.i a.out core $(EXTRA_CLEAN)

cleaner : clean
	- $(RM) pgtest cpgtest

realclean cleanest : cleaner
	- $(RM) $(PGSBOXLIB) $(LIBWCS)

check test : tests
	-@ echo ''
	-@ $(TIMER)
	-@ echo 'Running the PGSBOX test program, PGTEST:'
	-@ pgtest < /dev/null
	-@ echo ''
	-@ echo ''
	-@ $(TIMER)
	-@ echo 'Running the cpgsbox() test program, cpgtest:'
	-@ cpgtest < /dev/null

tests : wcslib pgtest cpgtest

wcslib : $(LIBWCS)

$(LIBWCS) :
	-@ echo ''
	   $(MAKE) -C ../FORTRAN lib

install : lib
	$(MAKE) -C ../FORTRAN install
	$(INSTALL) -m 644 $(PGSBOXLIB) $(LIBDIR)
	$(RM) $(LIBDIR)/libpgsbox.a
	$(LN_S) $(LIBDIR)/$(PGSBOXLIB) $(LIBDIR)/libpgsbox.a
	$(INSTALL) -m 444 CHANGES $(INCDIR)/CHANGES_PGSBOX
	$(INSTALL) -m 444 *.h $(INCDIR)

pgtest : pgtest.o lngvel.o fscan.o $(PGSBOXLIB) $(LIBWCS)
	-@ rm -f $@
	   $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $^ $(PGPLOTLIB) $(LIBS)

cpgtest : cpgtest.c lngvel.o fscan.o $(PGSBOXLIB) $(LIBWCS)
	-@ rm -f $@
	   $(CC) $(CPPFLAGS) -I. -I../C $(PGPLOTINC) $(CFLAGS) $(LDFLAGS) \
	   -o $@ $^ $(PGPLOTLIB) $(LIBS)

GNUmakefile : ../makedefs ;

../makedefs : ../makedefs.in ../config.status
	cd .. && config.status

# Dependencies
#-------------

$(PGSBOXLIB)(cpgsbox.o) : cpgsbox.h
$(PGSBOXLIB)(pgwcsl.o)  : cel.h lin.h prj.h spc.h sph.h spx.h tab.h wcs.h

pgtest.o  : wcs.inc
cpgtest.o : cel.h cpgsbox.h lin.h prj.h spc.h spx.h tab.h wcs.h wcsfix.h
