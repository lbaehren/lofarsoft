
noinst_LTLIBRARIES = libpgutil.la

include_HEADERS = ColourMap.h pgutil.h quickplot.h Plot3D.h BandpassPlotter.h

libpgutil_la_SOURCES = ColourMap.C pgutil.C quickimage.C Plot3D.C

libpgutil_la_LIBADD = @PGPLOT_LIBS@

bin_PROGRAMS = psrlst

LDADD = @PGPLOT_LIBS@

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@

