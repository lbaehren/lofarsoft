
lib_LTLIBRARIES = libapsr.la

libapsr_la_SOURCES = apsr_udp.c

dist_bin_SCRIPTS = apsr_rm.csh apsr_edit.csh apsr_reprocess.csh

bin_PROGRAMS = apsr_udpdb bat_to_utc apsr_udpdisk apsr_udpheader 

LDADD = libapsr.la $(top_builddir)/src/libpsrdada.la

INCLUDES = -I$(top_srcdir)/src

apsr_hdr=apsr_def.h apsr_udp.h apsr_udpdb.h apsr_udpdisk.h apsr_udpheader.h 

apsr_udpdisk_SOURCES=$(apsr_hdr) apsr_udpdisk.c

#############################################################################
#
# Depends on 3rdparty code
#
#############################################################################

if COMPILE_THIRD

bin_PROGRAMS += apsr_dbudp test_boardudp apsr_udpgenerator

apsr_dbudp_SOURCES=$(apsr_hdr) apsr_udpdb.c

LDADD += $(top_builddir)/3rdparty/libtimers.la
INCLUDES += -I$(top_srcdir)/3rdparty
AM_CFLAGS = -D$(ARCH)

endif


