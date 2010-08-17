#############################################################################
#
# Depends on 3rdparty code
#
#############################################################################

if COMPILE_THIRD

lib_LTLIBRARIES = libcaspsr.la

casper_hdr=bibob.h caspsr_def.h caspsr_udp.h caspsr_udpNdb.h caspsr_udpNnic.h caspsr_udpdb.h caspsr_udpheader.h caspsr_udpthreaded.h mulbibob.h

libcaspsr_la_SOURCES = $(casper_hdr) bibob.h bibob.c mulbibob.h mulbibob.c caspsr_udp.h caspsr_udp.c start_observation.c

bin_PROGRAMS = caspsr_udpdb bibob_terminal caspsr_udpdb caspsr_udpthreaded caspsr_udpNdb caspsr_udpheader caspsr_udpgen caspsr_udpNnic


LDADD = libcaspsr.la $(top_builddir)/src/libpsrdada.la $(top_builddir)/3rdparty/libtimers.la
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/3rdparty
AM_CFLAGS = -D$(ARCH)

caspsr_udpNnic_LDADD = $(LDADD) @PGPLOT_LIBS@

# INCLUDES += -I$(HOME)/packages/src/libpcap-0.9.8.20081128/


#############################################################################
#
# Depends on PGPLOT
#
#############################################################################

if HAVE_PGPLOT

bin_PROGRAMS += mulbibob_server
AM_CPPFLAGS = @PGPLOT_CFLAGS@
mulbibob_server_LDADD = $(LDADD) @PGPLOT_LIBS@
endif

endif
