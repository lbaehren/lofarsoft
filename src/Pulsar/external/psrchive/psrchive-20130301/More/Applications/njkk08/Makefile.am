
noinst_LTLIBRARIES = libnjkk08.la

dist_data_DATA = 2D_data.asc pa_error_matrix.asc

libnjkk08_la_SOURCES = njkk08.h linchi2.C linchi2.h \
	functions.C KaraFit.C KaraFit.h

check_PROGRAMS = test_functions

test_functions_SOURCES = test_functions.C
test_functions_LDADD = libnjkk08.la \
	$(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @GSL_CFLAGS@

