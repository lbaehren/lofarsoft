noinst_LTLIBRARIES = libiri.la

libiri_la_SOURCES = cira.for irisubne.for irifun.for iritecne.for igrf.for \
	iri2rm.f iri2rm.h

check_PROGRAMS = test_iri

LDADD = libiri.la

libiri_la_LIBADD = @FLIBS@

include $(top_srcdir)/config/Makefile.include

