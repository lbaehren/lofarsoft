
noinst_LTLIBRARIES = libresources.la

nobase_include_HEADERS = Pulsar/SimplePredictor.h Pulsar/GeneratorInterpreter.h

libresources_la_SOURCES = Generator_default.C SimplePredictor.C \
	Parameters_children.C Predictor_children.C Site_location.C \
	GeneratorInterpreter.C Generator_factory.C

bin_PROGRAMS = getHorizon

getHorizon_SOURCES = getHorizon.C

check_PROGRAMS = test_Parameters_factory test_Predictor_factory

test_Parameters_factory_SOURCES = test_Parameters_factory.C
test_Predictor_factory_SOURCES = test_Predictor_factory.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @TEMPO2_CFLAGS@

LDADD = libresources.la

if HAVE_TEMPO2
  LDADD += $(top_builddir)/Util/tempo2/libtempo_2.la
endif

LDADD += $(top_builddir)/Util/tempo/libtempo.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la @FLIBS@

