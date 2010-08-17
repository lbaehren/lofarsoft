
lib_LTLIBRARIES = libbpsr.la

bpsr_hdr=bpsr_def.h bpsr_udp.h ibob.h multibob.h

libbpsr_la_SOURCES = $(bpsr_hdr) ibob.c multibob.c start_observation.c

bin_PROGRAMS = bpsr_udpheader bpsr_udpdb bpsr_dbdisk bpsr_udpdisk \
	bpsr_dbmon ibob_telnet multibob_server

LDADD = libbpsr.la $(top_builddir)/src/libpsrdada.la

INCLUDES = -I$(top_srcdir)/src

#############################################################################
#
# Depends on 3rdparty code
#
#############################################################################

if COMPILE_THIRD

bin_PROGRAMS += bpsr_dbavg bpsr_udpgenerator

bpsr_dbavg_LDADD = $(LDADD) $(top_builddir)/3rdparty/libtimers.la 
bpsr_udpgenerator_LDADD = $(LDADD) $(top_builddir)/3rdparty/libtimers.la 

INCLUDES += -I$(top_srcdir)/3rdparty
AM_CFLAGS = -D$(ARCH)

endif

#############################################################################
#
# Depends on PGPLOT
#
#############################################################################

if HAVE_PGPLOT

bin_PROGRAMS += bpsr_dbplot bpsr_diskplot bpsr_bramplot
AM_CPPFLAGS = @PGPLOT_CFLAGS@
bpsr_dbplot_LDADD = $(LDADD) @PGPLOT_LIBS@
bpsr_diskplot_LDADD = $(LDADD) @PGPLOT_LIBS@
bpsr_bramplot_LDADD = $(LDADD) @PGPLOT_LIBS@
endif


