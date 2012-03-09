
noinst_LTLIBRARIES = libtempo.la

nobase_include_HEADERS = Pulsar/Predictor.h Pulsar/Parameters.h \
	Pulsar/Generator.h Pulsar/Site.h Pulsar/TextParameters.h \
	Pulsar/ParametersLookup.h

include_HEADERS = psrephem.h ephio_func.h psrephem_orbital.h \
	polyco.h Phase.h toa.h residual.h resio.h tempo++.h Predict.h \
	inverse_phase.h Observatory.h

nodist_include_HEADERS = ephio.h

libtempo_la_SOURCES = keys.dat ephio_c.c ephio.f \
	psrephem.C set_epoch.C tex.C derived.C psrephem_orbital.C \
	getlun.f length.f posparse.f strmatch.f upcase.f zeropad.f \
	polyco.C Phase.C toa.C residual.C resio.f \
	tempo++.C Predict.C fit.C inverse_phase.C \
	Observatory.C obsys.C itoa.C tempo_impl.h \
	Parameters.C TextParameters.C ParametersLookup.C

nodist_libtempo_la_SOURCES = ephio_def.c

bin_PROGRAMS = keys2c keys2fort ephepo ephtex ephorb fdot

ephepo_SOURCES = ephepo.C
ephtex_SOURCES = ephtex.C
ephorb_SOURCES = ephorb.C
fdot_SOURCES = fdot.C

check_PROGRAMS = test_ephio test_polyco_io test_Phase \
	test_get_configuration test_psrephem test_obsys $(TESTS)

TESTS = test_polyco_io test_psrephem test_TextParameters

test_polyco_io_SOURCES		= test_polyco_io.C test.polyco
test_Phase_SOURCES		= test_Phase.C
test_get_configuration_SOURCES	= test_get_configuration.C
test_psrephem_SOURCES		= test_psrephem.C test.psrephem
test_obsys_SOURCES		= test_obsys.C
test_TextParameters_SOURCES	= test_TextParameters.C

# ######################################################################
#
# Assembly of include files
#

keyinfo.com: $(srcdir)/keys.dat ./keys2fort
	./keys2fort $(srcdir)/keys.dat

ephio_def.c ephio.h: $(srcdir)/keys.dat ./keys2c
	./keys2c $(srcdir)/keys.dat

# Depend on constructed include files
ephio.lo: keyinfo.com
ephio_c.lo: ephio.h

BUILT_SOURCES = ephio.h keyinfo.com ephio_def.c

keys2fort_LDADD = 
keys2c_LDADD =  

#############################################################################
#

LDADD = libtempo.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la @FLIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @CFITSIO_CFLAGS@

#
# builddir != srcdir fix: g77 version 3 looks for header files
# (like keyinfo.com) only in the same path as the source code file.
#
AM_FFLAGS = -I.

#
# keep 'make distclean' happy
#
CLEANFILES += $(top_builddir)/Util/tempo/test.psrephem.out*

