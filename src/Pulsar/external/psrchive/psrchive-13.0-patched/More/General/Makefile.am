
noinst_LTLIBRARIES = libGeneral.la

dist_data_DATA = fluxcal.cfg

nobase_include_HEADERS = \
	find_spike_edges.h \
        model_profile.h \
        morphological_difference.h \
	Pulsar/Append.h \
	Pulsar/counter_drift.h \
	Pulsar/Accumulate.h \
        Pulsar/AdaptiveSNR.h \
        Pulsar/AdaptiveSmooth.h \
        Pulsar/Algorithm.h \
        Pulsar/ArchiveSort.h \
	Pulsar/ArchiveTemplates.h \
        Pulsar/BaselineEstimator.h \
	Pulsar/BaselineInterpreter.h \
        Pulsar/BaselineWindow.h \
        Pulsar/ChannelSubsetMatch.h \
        Pulsar/ChannelWeight.h \
        Pulsar/ChannelZapMedian.h \
        Pulsar/ChannelZapModulation.h \
        Pulsar/ColdPlasma.h \
	Pulsar/Combination.h \
	Pulsar/CommonOptions.h \
	Pulsar/Contemporaneity.h \
	Pulsar/ConvertIsolated.h \
	Pulsar/Differentiate.h \
	Pulsar/DisperseWeight.h \
        Pulsar/DispersionDelay.h \
        Pulsar/Dispersion.h \
	Pulsar/Divided.h \
        Pulsar/DurationWeight.h \
	Pulsar/EvenlySpaced.h \
	Pulsar/EvenlyWeighted.h \
        Pulsar/ExponentialBaseline.h \
	Pulsar/FixFluxCal.h \
	Pulsar/FortranSNR.h \
        Pulsar/Fourier.h \
        Pulsar/FourierSNR.h \
	Pulsar/FrequencyAppend.h \
        Pulsar/FrequencyIntegrate.h \
        Pulsar/GaussianBaseline.h \
	Pulsar/Index.h \
	Pulsar/Integrate.h \
	Pulsar/InstallInterpreter.h \
        Pulsar/IntegrationWeight.h \
        Pulsar/Interpreter.h \
	Pulsar/InterpreterExtension.h \
        Pulsar/IterativeBaseline.h \
	Pulsar/LastHarmonic.h \
	Pulsar/LastSignificant.h \
	Pulsar/MaskSmooth.h \
        Pulsar/NoiseStatistics.h \
        Pulsar/OnPulseEstimator.h \
        Pulsar/OnPulseThreshold.h \
	Pulsar/PatchTime.h \
	Pulsar/PeakEdgesInterpreter.h \
	Pulsar/PeakConsecutive.h \
	Pulsar/PeakCumulative.h \
        Pulsar/PhaseWeightFunction.h \
	Pulsar/PhaseWeightInterface.h \
	Pulsar/PhaseWeightModifier.h \
	Pulsar/PhaseWeightShift.h \
	Pulsar/PhaseWeightSmooth.h \
        Pulsar/PhaseWeight.h \
	Pulsar/ProfileInterpreter.h \
	Pulsar/ProfilePCA.h \
	Pulsar/ProfileShiftFit.h \
	Pulsar/ProfileStats.h \
	Pulsar/ProfileStatsInterface.h \
        Pulsar/ProfileWeightFunction.h \
	Pulsar/RadiometerWeight.h \
	Pulsar/RemoveBaseline.h \
	Pulsar/RiseFall.h \
        Pulsar/RFIMitigation.h \
        Pulsar/ScatteredPowerCorrection.h \
        Pulsar/SimPolnTiming.h \
        Pulsar/Smooth.h \
        Pulsar/SmoothMean.h \
        Pulsar/SmoothMedian.h \
	Pulsar/SmoothSinc.h \
	Pulsar/SNRatioInterpreter.h \
        Pulsar/SNRWeight.h \
        Pulsar/SquareWave.h \
	Pulsar/StandardOptions.h \
	Pulsar/StandardCandles.h \
        Pulsar/StandardSNR.h \
	Pulsar/Statistics.h \
	Pulsar/StatisticsInterface.h \
	Pulsar/Subtract.h \
        Pulsar/SyntheticPolnProfile.h \
	Pulsar/TimeAppend.h \
	Pulsar/TimeIntegrate.h \
        Pulsar/Transformation.h \
        Pulsar/Transposer.h \
        Pulsar/WaveletSmooth.h \
        Pulsar/WaveletTransform.h \
        Pulsar/Weight.h \
	Pulsar/WeightedFrequency.h \
	Pulsar/WeightInterpreter.h \
        Pulsar/ZapInterpreter.h

libGeneral_la_SOURCES = \
	counter_drift.C \
	find_spike_edges.C \
        model_profile.C \
	morphological_difference.C \
	Accumulate.C \
        AdaptiveSNR.C \
	Append.C \
	ArchiveTemplates.C \
        Archive_append.C \
	Archive_apply_model.C \
	Archive_baseline.C \
        Archive_bscrunch.C \
        Archive_centre.C \
        Archive_centre_max_bin.C \
        Archive_convert_state.C \
        Archive_dedisperse.C \
        Archive_defaraday.C \
        Archive_find.C \
        Archive_fold.C \
        Archive_fscrunch.C \
	Archive_good_model.C \
        Archive_invint.C \
        Archive_pscrunch.C \
        Archive_remove_baseline.C \
        Archive_rms_baseline.C \
        Archive_rotate.C \
        Archive_set_ephemeris.C \
        Archive_set_model.C \
        ArchiveSort.C \
	Archive_total.C \
        Archive_transform.C \
        Archive_tscrunch.C \
        Archive_update_model.C \
	Archive_weighted_frequency.C \
	BaselineEstimator.C \
	BaselineInterpreter.C \
        BaselineWindow.C \
        ChannelSubsetMatch.C \
        ChannelZapMedian.C \
        ChannelZapModulation.C \
	CommonOptions.C \
	ConvertIsolated.C \
	Contemporaneity.C \
	Differentiate.C \
	DisperseWeight.C \
        Dispersion.C \
        DispersionDelay.C \
        DurationWeight.C \
        ExponentialBaseline.C \
	FixFluxCal.C \
	FortranSNR.C \
        Fourier.C \
        FourierSNR.C \
	FrequencyAppend.C \
        FrequencyIntegrate.C \
        GaussianBaseline.C \
	Index.C \
	InstallInterpreter.C \
        Integration_cal_levels.C \
        Integration_convert_state.C \
        Integration_dedisperse.C \
        Integration_find.C \
        Integration_fscrunch.C \
        Integration_get_Stokes.C \
        Integration_invint.C \
        Integration_new_PolnProfile.C \
        Integration_remove_baseline.C \
        Integration_rotate.C \
        Integration_total.C \
        Integration_transform.C \
        Integration_weighted_frequency.C \
	IntegrationWeight.C \
	Interpreter.C \
        IterativeBaseline.C \
	LastHarmonic.C \
        LastSignificant.C \
	MaskSmooth.C \
        NoiseStatistics.C \
	OnPulseEstimator.C \
	OnPulseThreshold.C \
	PatchTime.C \
	PeakEdgesInterpreter.C \
	PeakConsecutive.C \
	PeakCumulative.C \
	PhaseWeightInterface.C \
	PhaseWeightModifier.C \
	PhaseWeightShift.C \
        PhaseWeight.C \
	ProfileInterpreter.C \
	ProfileShiftFit.C \
	ProfileStats.C \
	ProfileStatsInterface.C \
	Profile_baseline.C \
        Profile_convolve.C \
        Profile_dedisperse.C \
        Profile_fftconv.C \
        Profile_find_minmax_phase.C \
        Profile_find_peak_edges.C \
        Profile_find_transitions.C \
        Profile_shift.C \
        Profile_snr.C \
        Profile_stats.C \
        ProfileWeightFunction.C \
	RadiometerWeight.C \
	RemoveBaseline.C \
	RiseFall.C \
        RFIMitigation.C \
        ScatteredPowerCorrection.C \
        Smooth.C \
        SmoothMean.C \
        SmoothMedian.C \
	SmoothSinc.C \
	SNRatioInterpreter.C \
        SNRWeight.C \
        SquareWave.C \
	standard_interface.C \
	standard_shell.C \
	StandardOptions.C \
        StandardCandles.C \
        StandardSNR.C \
	Statistics.C \
	StatisticsInterface.C \
	Subtract.C \
	TimeAppend.C \
	TimeIntegrate.C \
        Transposer.C \
        Weight.C \
	WeightedFrequency.C \
	WeightInterpreter.C \
	ZapInterpreter.C


##############################################################################
#
# GSL-dependent code
#
if HAVE_GSL

libGeneral_la_SOURCES += WaveletSmooth.C WaveletTransform.C AdaptiveSmooth.C

if HAVE_CFITSIO

libGeneral_la_SOURCES += ProfilePCA.C

endif
endif

#############################################################################
#

INCLUDES = \
	-I$(top_srcdir)/More/Timing \
	-I$(top_srcdir)/More/Polarimetry \
	-I$(top_srcdir)/More/MEAL \
	-I$(top_srcdir)/More/RFIMitigation

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @TEMPO2_CFLAGS@ @CFITSIO_CFLAGS@ @GSL_CFLAGS@

