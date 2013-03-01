
noinst_LTLIBRARIES = libpuma.la

nobase_include_HEADERS = Pulsar/PuMaArchive.h

libpuma_la_SOURCES = PuMaArchive.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PUMA_CFLAGS@

libpuma_la_LIBADD = @PUMA_LIBS@

