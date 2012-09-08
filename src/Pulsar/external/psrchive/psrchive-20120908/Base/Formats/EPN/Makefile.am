
noinst_LTLIBRARIES = libepn.la

nobase_include_HEADERS = Pulsar/EPNArchive.h

libepn_la_SOURCES = EPNArchive.C epnio.c rwepn.f epnhdr.inc epnio.h

#############################################################################

include $(top_srcdir)/config/Makefile.include

#
# builddir != srcdir fix: unlike g77 version 3, gfortran does not look
# for header files (like epnhdr.inc) in the same path as the source code file.
#
AM_FFLAGS = -I$(srcdir)

