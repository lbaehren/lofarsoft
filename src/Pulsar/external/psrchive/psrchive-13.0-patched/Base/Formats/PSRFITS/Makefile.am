
noinst_LTLIBRARIES = libpsrfits.la

include_HEADERS = Pulsar/FITSArchive.h

dist_data_DATA = psrheader.fits

libpsrfits_la_SOURCES = setup_io.C setup_profiles.h \
	CalibratorExtensionIO.h FITSArchive.C \
	ProfileColumn.C Pulsar/ProfileColumn.h \
	unload_FITSHdrExtension.C unload_ObsExtension.C \
	unload_Receiver.C load_Receiver.C \
	unload_WidebandCorrelator.C load_WidebandCorrelator.C \
	unload_CalInfoExtension.C load_CalInfoExtension.C \
	unload_ITRFExtension.C load_ITRFExtension.C \
	unload_ProcHistory.C load_ProcHistory.C \
	unload_DigitiserStatistics.C load_DigitiserStatistics.C \
	unload_DigitiserCounts.C load_DigitiserCounts.C \
	unload_CalibratorExtension.C load_CalibratorExtension.C \
	unload_PolnCalibratorExtension.C load_PolnCalibratorExtension.C \
	unload_FluxCalibratorExtension.C load_FluxCalibratorExtension.C \
	unload_CalibratorStokes.C load_CalibratorStokes.C \
	unload_FITSSUBHdrExtension.C load_FITSSUBHdrExtension.C \
	unload_Passband.C load_Passband.C \
	unload_Pointing.C load_Pointing.C clean_Pointing_columns.C \
	unload_Integration.C load_Integration.C \
	unload_Predictor.C load_Predictor.C \
	unload_Parameters.C load_Parameters.C \
	unload_polyco.C load_polyco.C \
	unload_text.h load_text.h \
	delete_hdu.C update_history.C \
	unload_integrations.C load_integration_state.C \
	CalibratorExtensionIO.C FITSAgent.C \
	P236_reference_epoch_correction.C \
	fitsio_psrephem.C fitsio_polyco.C fitsio_tempo.h

if HAVE_TEMPO2
libpsrfits_la_SOURCES += unload_T2Predictor.C load_T2Predictor.C
endif

check_PROGRAMS = test_template test_psrephem_fitsio test_polyco_fitsio

TESTS = test_template

test_template_SOURCES = test_template.C
test_psrephem_fitsio_SOURCES = test_psrephem_fitsio.C
test_polyco_fitsio_SOURCES = test_polyco_fitsio.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @CFITSIO_CFLAGS@ @MPI_CFLAGS@ @TEMPO2_CFLAGS@

LDADD = libpsrfits.la $(top_builddir)/Util/libpsrutil.la @CFITSIO_LIBS@

