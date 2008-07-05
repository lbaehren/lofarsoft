#-----------------------------------------------------------------------------
#   GNU makefile for building WCSLIB 4.2.
#
#   Summary of the main targets
#   ---------------------------
#      all:       Do 'make all' in each subdirectory.
#      check:     Do 'make check' in each subdirectory.
#      install:   Do 'make install' in each subdirectory.
#      realclean (or cleanest): Do 'make realclean' in each subdirectory.
#      show_all:  Print the values of all variables used in this GNUmakefile.
#      writable:  Run chmod recursively to make all sources writable.
#
#   Notes:
#      1) If you need to make changes then preferably modify makedefs.in
#         instead.
#
#      2) Refer also to the GNUmakefiles in subdirectories, particularly
#         C/GNUmakefile.
#
#   Author: Mark Calabretta, Australia Telescope National Facility
#   http://www.atnf.csiro.au/~mcalabre/index.html
#   $Id: GNUmakefile,v 4.2 2005/09/23 04:55:25 cal103 Exp $
#-----------------------------------------------------------------------------
# Get configure settings.
include makedefs

SUBDIRS := C FORTRAN pgsbox
TSTDIRS := C FORTRAN pgsbox

# Is a Fortran compiler available?
ifeq "$(FC)" ""
  SUBDIRS := C FORTRAN
  TSTDIRS := C
endif

# Is PGPLOT available?
ifeq "$(PGPLOTINC)" ""
  SUBDIRS := $(filter-out pgsbox, $(SUBDIRS))
endif

ifeq "$(PGPLOTLIB)" ""
  TSTDIRS := $(filter-out pgsbox, $(TSTDIRS))
endif


.PHONY : all check chmod cleanest install realclean show_all tests

all : show_all
	-@ for DIR in $(SUBDIRS) ; do \
	     echo '' ; \
	     $(TIMER) ; \
	     $(MAKE) -k -C $$DIR lib ; \
	   done

check tests : show_all
	-@ echo ''
	-@ $(TIMER)
	 @ for DIR in $(SUBDIRS) ; do \
	     echo '' ; \
	     $(MAKE) -i -C $$DIR FLAVOUR=$$FLAVOUR cleaner ; \
	   done
	 @ for FLAVOUR in $(FLAVOURS) ; do \
	     echo '' ; \
	     for DIR in $(TSTDIRS) ; do \
	       echo '' ; \
	       $(TIMER) ; \
	       $(MAKE) -k -C $$DIR FLAVOUR=$$FLAVOUR $@ ; \
	     done ; \
	     echo '' ; \
	     $(TIMER) ; \
	     for DIR in $(SUBDIRS) ; do \
	       echo '' ; \
	       $(MAKE) -i -C $$DIR FLAVOUR=$$FLAVOUR cleaner ; \
	     done ; \
	   done

install :
	$(MAKE) -k -C pgsbox $@
	$(INSTALL) -m 444 COPYING $(INCDIR)/COPYING
	$(INSTALL) -m 444 README  $(INCDIR)/README

realclean cleanest :
	for DIR in $(SUBDIRS) ; do \
	   $(MAKE) -C $$DIR realclean ; \
	done
	$(RM) -r autom4te.cache autoscan.log
	$(RM) config.log config.status configure.lineno makedefs C/config.h
	$(RM) *.log wcslib-*.tar.gz

show_all ::
	-@ echo 'Subdirectories to be built...'
	-@ echo '  SUBDIRS     := $(SUBDIRS)'
	-@ echo '  TSTDIRS     := $(TSTDIRS)'
	-@ echo '  FLAVOURS    := $(FLAVOURS)'
	-@ echo ''

writable :
	  chmod -R u+w .

GNUmakefile : makedefs ;

makedefs : makedefs.in config.status
	-@ echo ''
	-@ $(TIMER)
	   config.status

config.status : configure
	-@ echo ''
	-@ $(TIMER)
	   configure --no-create

#-----------------------------------------------------------------------------
# These are for code management.

.PHONY : autoconf dist

dist : autoconf realclean
	-@ echo $(WCSLIBV)/C/RCS       >  wcslib.X
	-@ echo $(WCSLIBV)/C/test/RCS  >> wcslib.X
	-@ echo $(WCSLIBV)/FORTRAN/RCS >> wcslib.X
	-@ echo $(WCSLIBV)/FORTRAN/test/RCS >> wcslib.X
	-@ echo $(WCSLIBV)/makedefs    >> wcslib.X
	-@ echo $(WCSLIBV)/RCS         >> wcslib.X
	-@ echo $(WCSLIBV)/other       >> wcslib.X
	-@ echo $(WCSLIBV)/pgsbox/RCS  >> wcslib.X
	-@ echo $(WCSLIBV)/wcslib.X    >> wcslib.X
	   tar cvfX ../$(WCSLIBV).tar wcslib.X -C .. $(WCSLIBV)
	   mv ../$(WCSLIBV).tar ./$(WCSLIBV).tar
	   rm -f wcslib.X $(WCSLIBV).tar.gz
	   gzip --best $(WCSLIBV).tar
	   chmod 444 $(WCSLIBV).tar.gz

autoconf : configure C/config.h.in

configure : configure.ac
	-@ echo ''
	-@ $(TIMER)
	   autoconf

C/config.h.in : configure.ac
	-@ echo ''
	-@ $(TIMER)
	   autoheader
