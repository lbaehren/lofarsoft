
noinst_LTLIBRARIES = libExtensions.la

nobase_include_HEADERS = \
        Pulsar/Arecibo.h \
        Pulsar/Backend.h \
	Pulsar/BackendInterface.h \
        Pulsar/BinaryPhaseOrder.h \
        Pulsar/BinLngAscOrder.h \
        Pulsar/BinLngPeriOrder.h \
        Pulsar/CalibratorExtension.h \
	Pulsar/CalibratorExtensionInterface.h \
        Pulsar/CalibratorStokes.h \
        Pulsar/CalInfoExtension.h \
        Pulsar/ColdPlasmaHistory.h \
        Pulsar/Dedisperse.h \
        Pulsar/DeFaraday.h \
        Pulsar/DigitiserStatistics.h \
	Pulsar/DigitiserCounts.h \
        Pulsar/dspReduction.h \
	Pulsar/Effelsberg.h \
        Pulsar/ExampleExtension.h \
        Pulsar/FeedExtension.h \
        Pulsar/FITSHdrExtension.h \
	Pulsar/FITSSUBHdrExtension.h \
        Pulsar/FluxCalibratorExtension.h \
        Pulsar/GBT.h \
        Pulsar/IntegrationOrder.h \
        Pulsar/ITRFExtension.h \
        Pulsar/ObsExtension.h \
        Pulsar/Parkes.h \
        Pulsar/Passband.h \
        Pulsar/PeriastronOrder.h \
        Pulsar/Pointing.h \
        Pulsar/PolnCalibratorExtension.h \
        Pulsar/PowerEstimator.h \
        Pulsar/ProcHistory.h \
        Pulsar/Receiver.h \
	Pulsar/Receiver_Field.h \
        Pulsar/Receiver_Linear.h \
        Pulsar/Receiver_Native.h \
        Pulsar/TapeInfo.h \
        Pulsar/Telescope.h \
        Pulsar/Telescopes.h \
        Pulsar/TimeSortedOrder.h \
        Pulsar/TwoBitStats.h \
        Pulsar/WidebandCorrelator.h \
	Pulsar/ExtensionRegister.h

libExtensions_la_SOURCES = \
        Arecibo.C \
        Backend.C \
        BinaryPhaseOrder.C \
        BinLngAscOrder.C \
        BinLngPeriOrder.C \
        CalibratorExtension_build.C \
        CalibratorExtension.C \
        CalibratorStokes.C \
        CalInfoExtension.C \
        ColdPlasmaHistory.C \
        Dedisperse.C \
        DeFaraday.C \
        DigitiserStatistics.C \
	DigitiserCounts.C \
        dspReduction.C \
	dspReductionTI.C \
	Effelsberg.C \
        ExampleExtension.C \
        FeedExtension.C \
        FITSHdrExtension.C \
	FITSHdrExtensionTI.C \
	FITSSUBHdrExtension.C \
	FITSSUBHdrExtensionTI.C \
        FluxCalibratorExtension.C \
        FluxCalibratorExtensionTI.C \
        GBT.C \
        IntegrationOrder.C \
        ITRFExtension.C \
        ObsExtension.C \
	ObsExtensionTI.C \
        Parkes.C \
        Passband.C \
        PeriastronOrder.C \
        Pointing.C \
        PointingTI.C \
        PolnCalibratorExtension.C \
        PolnCalibratorExtensionTI.C \
        ProcHistory.C \
        ProcHistoryTI.C \
        Receiver.C \
        Receiver_Field.C \
        Receiver_Linear.C \
        Receiver_Native.C \
	Receiver_load.C \
        TapeInfo.C \
        Telescope.C \
        Telescopes.C \
        TimeSortedOrder.C \
        TwoBitStats.C \
        WidebandCorrelator.C \
        WidebandCorrelatorTI.C \
	ExtensionRegister.C

#############################################################################
#

INCLUDES = -I$(top_srcdir)/More/Polarimetry

include $(top_srcdir)/config/Makefile.include

