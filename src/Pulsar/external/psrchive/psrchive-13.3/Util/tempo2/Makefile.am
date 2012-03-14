
noinst_LTLIBRARIES = libtempo_2.la

dist_data_DATA = tempo2.key

include_HEADERS = T2Generator.h T2Predictor.h T2Parameters.h T2Observatory.h

libtempo_2_la_SOURCES = \
	T2Predictor.C T2Generator.C T2Parameters.C T2Observatory.C

check_PROGRAMS = test_T2Generator

test_T2Generator_SOURCES = test_T2Generator.C

#############################################################################
#

LDADD = libtempo_2.la \
	$(top_builddir)/Util/tempo/libtempo.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la @FLIBS@

libtempo_2_la_LIBADD = @TEMPO2_LIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @TEMPO2_CFLAGS@

