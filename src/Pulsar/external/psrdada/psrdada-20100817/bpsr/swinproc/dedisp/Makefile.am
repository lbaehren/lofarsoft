
lib_LTLIBRARIES = libdedisp.la

libdedisp_la_SOURCES = getDMtable.C gtools.C

bin_PROGRAMS = dedisperse_all

dedisperse_all_SOURCES = dedisperse_all.C dedisperse_header.c

LDADD = $(top_builddir)/bpsr/swinproc/libswinproc.la libdedisp.la

dedisperse_all_LDADD = $(LDADD)

INCLUDES = -I$(top_srcdir)/bpsr/swinproc

AM_CXXFLAGS = @OPENMP_CFLAGS@

