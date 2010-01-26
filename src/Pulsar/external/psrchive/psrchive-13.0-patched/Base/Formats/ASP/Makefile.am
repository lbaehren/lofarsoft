
noinst_LTLIBRARIES = libasp.la

include_HEADERS = Pulsar/ASPArchive.h

libasp_la_SOURCES = ASPArchive.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @CFITSIO_CFLAGS@

libasp_la_LIBADD = @CFITSIO_LIBS@

