
noinst_LTLIBRARIES = libPolarimetry.la

nobase_include_HEADERS = \
	Pulsar/BackendCorrection.h \
	Pulsar/BackendFeed.h \
	Pulsar/BackendFeedInfo.h \
	Pulsar/BasisCorrection.h \
	Pulsar/Britton2000.h \
	Pulsar/BrittonInfo.h \
        Pulsar/CalibratorPlotter.h \
        Pulsar/CalibratorStokesInfo.h \
	Pulsar/CalInterpreter.h \
        Pulsar/CoherencyMeasurement.h \
        Pulsar/CoherencyMeasurementSet.h \
	Pulsar/ComplexRVMFit.h \
        Pulsar/ConvertMJD.h \
        Pulsar/Database.h \
        Pulsar/DeltaPA.h \
        Pulsar/DeltaRM.h \
        Pulsar/DoPCalibrator.h \
        Pulsar/Faraday.h \
        Pulsar/FaradayRotation.h \
        Pulsar/Feed.h \
        Pulsar/FluxCalibratorData.h \
        Pulsar/FluxCalibrator.h \
	Pulsar/FrontendCorrection.h \
        Pulsar/HybridCalibrator.h \
        Pulsar/Instrument.h \
	Pulsar/InstrumentInfo.h \
        Pulsar/IonosphereCalibrator.h \
        Pulsar/IRIonosphere.h \
        Pulsar/MeanCoherency.h \
        Pulsar/MeanFeed.h \
        Pulsar/MeanInstrument.h \
	Pulsar/MeanJones.h \
        Pulsar/MeanPolar.h \
        Pulsar/MeanSingleAxis.h \
        Pulsar/MeasurementEquation.h \
        Pulsar/ObservationUncertainty.h \
        Pulsar/OffPulseCalibrator.h \
        Pulsar/Parallactic.h \
        Pulsar/PolarCalibrator.h \
        Pulsar/Polarization.h \
        Pulsar/PolnCalibrator.h \
        Pulsar/PolnProfile.h \
        Pulsar/PolnProfileFit.h \
        Pulsar/PolnProfileStats.h \
	Pulsar/PolnSpectrumStats.h \
	Pulsar/PolynomialInfo.h \
	Pulsar/ProjectionCorrection.h \
        Pulsar/PulsarCalibrator.h \
        Pulsar/ReceptionCalibrator.h \
        Pulsar/ReceptionModelAxisPlotter.h \
        Pulsar/ReceptionModel.h \
        Pulsar/ReceptionModelPlotter.h \
	Pulsar/ReceptionModelReport.h \
	Pulsar/ReceptionModelSolver.h \
	Pulsar/ReceptionModelSolveGSL.h \
	Pulsar/ReceptionModelSolveMEAL.h \
        Pulsar/ReferenceCalibrator.h \
        Pulsar/ReflectStokes.h \
        Pulsar/Simulation.h \
        Pulsar/SingleAxisCalibrator.h \
        Pulsar/SingleAxis.h \
        Pulsar/SingleAxisPolynomial.h \
        Pulsar/SingleAxisSolver.h \
	Pulsar/SolverInfo.h \
	Pulsar/SourceInfo.h \
	Pulsar/StandardData.h \
        Pulsar/StandardModel.h \
	Pulsar/StandardPrepare.h \
	Pulsar/StandardSpectra.h \
	Pulsar/StepsInfo.h \
	Pulsar/StokesCovariance.h \
        Pulsar/SystemCalibrator.h \
	Pulsar/SystemCalibratorPlotter.h \
	Pulsar/SystemCalibratorUnloader.h \
        Pulsar/TemplateUncertainty.h \
        Pulsar/TotalCovariance.h \
	Pulsar/VariationInfo.h

libPolarimetry_la_SOURCES = \
	BackendCorrection.C \
	BackendFeed.C \
	BackendFeedInfo.C \
	BasisCorrection.C \
	Britton2000.C \
	BrittonInfo.C \
        CalibratorStokesInfo.C \
	CalibratorType_factory.C \
	CalInterpreter.C \
        choose.C \
        CoherencyMeasurement.C \
        CoherencyMeasurementSet.C \
	ComplexRVMFit.C \
        ConvertMJD.C \
        Database.C \
        DeltaPA.C \
        DeltaRM.C \
        DoPCalibrator.C \
        Faraday.C \
        FaradayRotation.C \
        Feed.C \
        Feed_load.C \
        FluxCalibrator.C \
        FluxCalibratorData.C \
	FluxCalibratorExt.C \
	FrontendCorrection.C \
        HybridCalibrator.C \
        Instrument.C \
        InstrumentInfo.C \
        Integration_defaraday.C \
        IonosphereCalibrator.C \
        IRIonosphere.C \
        MeanCoherency.C \
        MeanFeed.C \
        MeanInstrument.C \
	MeanJones.C \
        MeanPolar.C \
        MeanSingleAxis.C \
        MeasurementEquation.C \
        ObservationUncertainty.C \
        OffPulseCalibrator.C \
        Parallactic.C \
        PolarCalibrator.C \
        Polarization.C \
        Polar_solve.C \
        PolnCalibrator.C \
        PolnCalibratorExt.C \
        PolnCalibratorInfo.C \
        PolnProfile.C \
        PolnProfileFit.C \
        PolnProfileStats.C \
	PolnSpectrumStats.C \
	PolynomialInfo.C \
	ProjectionCorrection.C \
        PulsarCalibrator.C \
        ReceptionCalibrator.C \
        ReceptionModel.C \
	ReceptionModelReport.C \
	ReceptionModelSolver.C \
	ReceptionModelSolveMEAL.C \
        ReferenceCalibrator.C \
        ReflectStokes.C \
        Simulation.C \
        SingleAxis.C \
        SingleAxisCalibrator.C \
        SingleAxisPolynomial.C \
        SingleAxisSolver.C \
	SolverInfo.C \
        SourceInfo.C \
	StandardData.C \
        StandardModel.C \
	StandardPrepare.C \
	StandardSpectra.C \
	StepsInfo.C \
	StokesCovariance.C \
        SystemCalibrator.C \
	SystemCalibratorUnloader.C \
        TemplateUncertainty.C \
        TotalCovariance.C \
	VariationInfo.C

if HAVE_GSL
libPolarimetry_la_SOURCES += ReceptionModelSolveGSL.C
endif

SUBDIRS = iri2001

libPolarimetry_la_LIBADD = iri2001/libiri.la @GSL_LIBS@

bin_PROGRAMS = rmrot simpol

rmrot_SOURCES = rmrot.C
simpol_SOURCES = simpol.C

##############################################################################
#
# PGPLOT-specific code
#
if HAVE_PGPLOT

libPolarimetry_la_SOURCES += \
	CalibratorPlotter.C \
	SystemCalibratorPlotter.C \
	ReceptionModelPlotter.C

bin_PROGRAMS += pafit

pafit_SOURCES		= pafit.C 
pafit_LDADD		= $(LDADD) @PGPLOT_LIBS@

endif

# ######################################################################
#
# test programs
#

TESTS = test_copy test_Feed test_SingleAxis test_TotalCovariance \
	test_Parallactic test_ReceptionComposite test_ReceptionEvaluate \
	test_ReceptionModel test_Instrument \
	test_hand_xyph 

check_PROGRAMS = $(TESTS) test_IRIonosphere

test_ReceptionComposite_SOURCES	= test_ReceptionComposite.C
test_ReceptionEvaluate_SOURCES	= test_ReceptionEvaluate.C

test_Feed_SOURCES		= test_Feed.C
test_Parallactic_SOURCES	= test_Parallactic.C
test_SingleAxis_SOURCES		= test_SingleAxis.C
test_TotalCovariance_SOURCES	= test_TotalCovariance.C
test_Instrument_SOURCES		= test_Instrument.C
test_hand_xyph_SOURCES		= test_hand_xyph.C

test_ReceptionModel_SOURCES	= test_ReceptionModel.C

test_copy_SOURCES		= test_copy.C
test_IRIonosphere_SOURCES	= test_IRIonosphere.C

#############################################################################
#

INCLUDES = -I$(top_srcdir)/More/Plotting

LDADD = libPolarimetry.la \
	$(top_builddir)/More/MEAL/libMEAL.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @GSL_CFLAGS@

