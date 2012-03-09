
lib_LTLIBRARIES = libpsrchive.la

nobase_include_HEADERS = model_profile.h	\
	Pulsar/ArchiveExpert.h			\
	Pulsar/Algorithm.h			\
	Pulsar/Container.h			\
	Pulsar/AdaptiveSNR.h			\
	Pulsar/Archive.h			\
	Pulsar/ArchiveMatch.h			\
	Pulsar/ArchiveSort.h			\
	Pulsar/ArchiveProcessor.h		\
	Pulsar/PolnProfile.h			\
	Pulsar/BaselineEstimator.h		\
	Pulsar/DeltaPA.h			\
	Pulsar/DeltaRM.h			\
	Pulsar/ExponentialBaseline.h		\
	Pulsar/GaussianBaseline.h		\
	Pulsar/Profile.h			\
	Pulsar/BasicArchive.h			\
	Pulsar/BasicIntegration.h		\
	Pulsar/RFIMitigation.h			\
	Pulsar/ColdPlasma.h			\
	Pulsar/Dispersion.h			\
	Pulsar/DispersionDelay.h		\
	Pulsar/Fourier.h			\
	Pulsar/IntegrationManagerExpert.h	\
	Pulsar/IntegrationExpert.h		\
	Pulsar/Transformation.h			\
	Pulsar/Smooth.h				\
	Pulsar/ExampleArchive.h			\
	Pulsar/SmoothMean.h			\
	Pulsar/Polarization.h			\
	Pulsar/SmoothMedian.h			\
	Pulsar/FourierSNR.h			\
	Pulsar/StandardSNR.h			\
	Pulsar/OnPulseThreshold.h		\
	Pulsar/NoiseStatistics.h		\
	Pulsar/OnPulseEstimator.h		\
	Pulsar/PolnProfileStats.h		\
	Pulsar/InterpreterVariables.h		\
	Pulsar/SquareWave.h			\
	Pulsar/ScatteredPowerCorrection.h	\
	Pulsar/Integration.h			\
	Pulsar/Transposer.h			\
	Pulsar/IntegrationManager.h		\
	Pulsar/Utilities.h			\
	Pulsar/Interpreter.h			\
	Pulsar/psrchive.h			\
	Pulsar/IterativeBaseline.h		\
	Pulsar/PhaseWeight.h			\
	Pulsar/ArchiveTI.h			\
	Pulsar/PhaseWeightFunction.h		\
	Pulsar/ProfileWeightFunction.h		\
	Pulsar/Plotter.h			\
	Pulsar/IntegrationTI.h			\
	Pulsar/BaselineWindow.h			\
	Pulsar/Pulsar.h				\
	Pulsar/ReflectStokes.h			\
	Pulsar/ChannelZapMedian.h		\
	Pulsar/ChannelWeight.h			\
	Pulsar/ChannelZapModulation.h		\
	Pulsar/Config.h				\
	Pulsar/Profile_utils.h			\
	Pulsar/Archive_utils.h			\
	Pulsar/Weight.h				\
	Pulsar/IntegrationWeight.h		\
	Pulsar/DurationWeight.h			\
	Pulsar/SNRWeight.h

libpsrchive_la_SOURCES =			\
	AdaptiveSNR.C				\
	Agent.C					\
	Agent_static.C				\
	Archive_append.C			\
	Archive.C				\
	Archive_convert_state.C			\
	Archive_copy.C				\
	Archive_denoise_method.C		\
	Archive_fappend.C			\
	Archive_find.C				\
	Archive_get_PA.C			\
	Archive_init_Integration.C		\
	Archive_load.C				\
	Archive_match.C				\
	ArchiveMatch.C				\
	Archive_resize.C			\
	ArchiveSort.C				\
	Archive_split_method.C			\
	ArchiveTI.C				\
	Archive_toas.C				\
	Archive_transform.C			\
	Archive_unload.C			\
	Archive_update_model.C			\
	Archive_utils.C				\
	Archive_verbose.C			\
	BaselineWindow.C			\
	BasicArchive.C				\
	BasicIntegration.C			\
	ChannelZapMedian.C			\
	ChannelZapModulation.C			\
	choose.C				\
	DeltaPA.C				\
	DeltaRM.C				\
	Dispersion.C				\
	DispersionDelay.C			\
	DurationWeight.C			\
	ExampleArchive.C			\
	ExponentialBaseline.C			\
	Fourier.C				\
	FourierSNR.C				\
	GaussianBaseline.C			\
	getopt.h				\
	Integration.C				\
	Integration_cal_levels.C		\
	Integration_convert_state.C		\
	Integration_dedisperse.C		\
	Integration_fappend.C			\
	Integration_find.C			\
	Integration_get_PA.C			\
	Integration_get_Stokes.C		\
	Integration_invint.C			\
	IntegrationManager.C			\
	Integration_match.C			\
	Integration_new_PolnProfile.C		\
	Integration_remove_baseline.C		\
	Integration_resize.C			\
	IntegrationTI.C				\
	Integration_toas.C			\
	Integration_total.C			\
	Integration_transform.C			\
	Integration_weighted_frequency.C	\
	Interpreter.C				\
	InterpreterVariables.C			\
	IterativeBaseline.C			\
	LastSignificant.C			\
	model_profile.C				\
	mtm_analysis.C				\
	NoiseStatistics.C			\
	OnPulseThreshold.C			\
	pafit_get_PA.C				\
	PhaseGradShift.C			\
	PhaseWeight.C				\
	Physical.C				\
	Physical.h				\
	plugins.csh				\
	Polarization.C				\
	PolnProfile.C				\
	PolnProfileStats.C			\
	Profile_average.C			\
	Profile.C				\
	Profile_convolve.C			\
	Profile_denoise.C			\
	Profile_fftconv.C			\
	Profile_find_minmax_phase.C		\
	Profile_find_peak_edges.C		\
	Profile_find_transitions.C		\
	Profile_flux.C				\
	Profile_rotate.C			\
	Profile_shift.C				\
	Profile_snr.C				\
	Profile_snr_fortran.C			\
	Profile_stats.C				\
	Profile_toa.C				\
	Profile_utils.C				\
	ProfileWeightFunction.C			\
	ReflectStokes.C				\
	RFIMitigation.C				\
	ScatteredPowerCorrection.C		\
	SincInterpShift.C			\
	Smooth.C				\
	SmoothMean.C				\
	SmoothMedian.C				\
	SNRWeight.C				\
	SquareWave.C				\
	StandardSNR.C				\
	Transposer.C				\
	Utilities.C				\
	Weight.C				\
	ZeroPadShift.C

if !HAVE_GETOPT_LONG
libpsrchive_la_SOURCES += getopt.c getopt1.c
endif

SUBDIRS = Extension timer epn ascii

libpsrchive_la_LIBADD = Extension/libExtension.la			  \
			timer/libtimer.la epn/libepn.la ascii/libascii.la

# ######################################################################
#
# programs
#
bin_PROGRAMS = psredit psrconv vip psr_template \
	fix_coords fix_fluxcal fix_hydra fix_3C353 \
	sesdiv parInfo default_loader archive_correct peb

psredit_SOURCES			= psredit.C
psrconv_SOURCES			= psrconv.C
vip_SOURCES			= vip.C
psr_template_SOURCES		= psr_template.C
fix_coords_SOURCES		= fix_coords.C
fix_fluxcal_SOURCES		= fix_fluxcal.C
fix_hydra_SOURCES		= fix_hydra.C
fix_3C353_SOURCES		= fix_3C353.C
sesdiv_SOURCES			= sesdiv.C
parInfo_SOURCES                 = parInfo.C
default_loader_SOURCES		= default_loader.C
archive_correct_SOURCES		= archive_correct.C
peb_SOURCES	         	= peb.C

# ######################################################################
#
# The following codes make use of MEAL
#

if !MAKE_LEAN

nobase_include_HEADERS +=			\
	Pulsar/Calibrator.h			\
	Pulsar/CalibratorPlotter.h		\
	Pulsar/CalibratorStokesInfo.h		\
	Pulsar/CorrectionsCalibrator.h		\
	Pulsar/Database.h			\
	Pulsar/DoPCalibrator.h			\
	Pulsar/FaradayRotation.h		\
	Pulsar/FluxCalibratorData.h		\
	Pulsar/FluxCalibratorDatabase.h		\
	Pulsar/FluxCalibrator.h			\
	Pulsar/FrequencyIntegrate.h		\
	Pulsar/HybridCalibrator.h		\
	Pulsar/InstrumentInfo.h			\
	Pulsar/IonosphereCalibrator.h		\
	Pulsar/OffPulseCalibrator.h		\
	Pulsar/PolarCalibrator.h		\
	Pulsar/PolnCalibrator.h			\
	Pulsar/PolnProfileFit.h			\
	Pulsar/PolnProfileFitAnalysis.h		\
	Pulsar/PulsarCalibrator.h		\
	Pulsar/ReceptionCalibrator.h		\
	Pulsar/ReceptionCalibratorPlotter.h	\
	Pulsar/ReferenceCalibrator.h		\
	Pulsar/SimPolnTiming.h			\
	Pulsar/SingleAxisCalibrator.h		\
	Pulsar/SourceInfo.h			\
	Pulsar/SyntheticPolnProfile.h		\
	Pulsar/SyntheticProfile.h		\
	Pulsar/SystemCalibrator.h

libpsrchive_la_SOURCES +=			\
	Archive_fscrunch.C			\
	Archive_centre_max_bin.C		\
	Archive_defaraday.C			\
	Archive_find_best_period.C		\
	Archive_get_poln_flux.C			\
	Archive_invint.C			\
	Archive_remove_baseline.C		\
	Archive_rms_baseline.C			\
	Archive_tscrunch.C Archive_total.C	\
	Integration_flux.C			\
	Integration_fscrunch.C			\
	Archive_correct_instrument.C		\
	Calibrator.C				\
	CalibratorStokesInfo.C			\
	CorrectionsCalibrator.C			\
	Database.C				\
	DoPCalibrator.C				\
	FaradayRotation.C			\
	FluxCalibrator.C			\
	FluxCalibratorDatabase.C		\
	FluxCalibratorData.C			\
	FrequencyIntegrate.C			\
	GaussianShift.C				\
	HybridCalibrator.C			\
	InstrumentInfo.C			\
	Integration_defaraday.C			\
	IonosphereCalibrator.C			\
	OffPulseCalibrator.C			\
	PolarCalibrator.C			\
	PolnCalibrator.C			\
	PolnCalibratorInfo.C			\
	PolnCalibratorExt.C			\
	PolnProfileFit.C			\
        PolnProfileFitAnalysis.C		\
	Profile_basic_shift.C			\
	PulsarCalibrator.C			\
	Receiver_get_transformation.C		\
	Receiver_load.C				\
	ReceptionCalibrator.C			\
	ReceptionCalibrator_get_Info.C		\
	ReceptionCalibrator_get_solution.C	\
	ReferenceCalibrator.C			\
	SimPolnTiming.C				\
	SingleAxisCalibrator.C			\
	SourceInfo.C				\
	SyntheticProfile.C			\
	SyntheticPolnProfile.C			\
	SystemCalibrator.C 			\
	MTM_optimize.C dfpmin.c lnsrch.c nrutil.c

SUBDIRS += Calibration

libpsrchive_la_LIBADD += Calibration/libCalibration.la

bin_PROGRAMS += vap pam psrsh psradd pascii paz pac pat pcq fluxcal	\
	fadd archive_denoise archive_split mtm mtm_unique

vap_SOURCES			= vap.C
pam_SOURCES			= pam.C
psrsh_SOURCES			= psrsh.C
psrsh_LDADD			= $(LDADD) @READLINE_LIBS@
psradd_SOURCES			= psradd.C
pascii_SOURCES			= pascii.C
paz_SOURCES			= paz.C
pac_SOURCES			= pac.C
pat_SOURCES			= pat.C

mtm_SOURCES			= mtm.C
mtm_unique_SOURCES		= mtm_unique.C

pcq_SOURCES			= pcq.C
fluxcal_SOURCES			= fluxcal.C
fadd_SOURCES			= fadd.C
archive_denoise_SOURCES		= archive_denoise.C
archive_split_SOURCES		= archive_split.C

TESTS = test_Receiver test_Interpreter test_DeltaPA

check_PROGRAMS = test_Archive test_denoise test_Difference $(TESTS)

test_Receiver_SOURCES		= test_Receiver.C
test_Interpreter_SOURCES	= test_Interpreter.C
test_DeltaPA_SOURCES		= test_DeltaPA.C
test_Archive_SOURCES		= test_Archive.C
test_denoise_SOURCES		= test_denoise.C
test_Difference_SOURCES		= test_Difference.C

endif

if HAVE_CFITSIO
SUBDIRS += psrfits
libpsrchive_la_LIBADD += psrfits/libpsrfits.la
endif

if HAVE_PGPLOT
SUBDIRS += Plotter
libpsrchive_la_LIBADD += Plotter/libPlot.la
endif

SUBDIRS += .

if HAVE_QT
if HAVE_PGPLOT
SUBDIRS += GUI
endif
endif

if HAVE_QTDRIV
SUBDIRS += rhythm
endif

# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

libpsrchive_la_SOURCES += ArchiveProcessor.C Plotter.C CalibratorPlotter.C \
	ReceptionCalibratorPlotter.C   plotJones.C			   \
	Plotter_Mercator.C             Plotter_phase_subints.C		   \
	Plotter_baseline_spectrum.C    Plotter_plot.C			   \
	Plotter_calibrator_spectrum.C  Plotter_set_colour_map.C		   \
	Plotter_draw_transitions.C     Plotter_simple_ds.C		   \
	Plotter_phase_fourier.C        Plotter_spherical.C		   \
	Plotter_phase_frequency.C      Plotter_phase_time.C		   \
	Plotter_draw_colour_map.C      Plotter_Manchester.C

bin_PROGRAMS += psrplot pacv pcvar pav pas psrwt pcm pvt paf psrqft \
	binphs_test pcm_chooser rmfit psrband SimTiming bin_clean \
	plot_baseline pdmp psrfi tsys spa psrchive_template paas \
	pfixbaseline pcmdiff psrds psrdiff

SimTiming_SOURCES		= SimTiming.C
binphs_test_SOURCES		= binphs_test.C
pacv_SOURCES			= pacv.C
pas_SOURCES			= pas.C
pav_SOURCES			= pav.C
pcm_SOURCES			= pcm.C
pcm_chooser_SOURCES		= pcm_chooser.C
pcvar_SOURCES			= pcvar.C
psrband_SOURCES			= psrband.C
psrplot_SOURCES			= psrplot.C
psrwt_SOURCES			= psrwt.C
pvt_SOURCES			= pvt.C
rmfit_SOURCES			= rmfit.C
bin_clean_SOURCES		= bin_clean.C
paf_SOURCES			= paf.C
psrqft_SOURCES			= psrqft.C
plot_baseline_SOURCES		= plot_baseline.C
pdmp_SOURCES                    = pdmp.C
psrfi_SOURCES                   = psrfi.C
spa_SOURCES                     = spa.C
tsys_SOURCES                    = tsys.C
paas_SOURCES			= paas.C
pfixbaseline_SOURCES		= pfixbaseline.C
psrchive_template_SOURCES       = psrchive_template.C
pcmdiff_SOURCES			= pcmdiff.C
psrds_SOURCES			= psrds.C
psrdiff_SOURCES                 = psrdiff.C

SimTiming_LDADD			= $(LDADD) @PGPLOT_LIBS@
binphs_test_LDADD		= $(LDADD) @PGPLOT_LIBS@
pacv_LDADD			= $(LDADD) @PGPLOT_LIBS@
pas_LDADD			= $(LDADD) @PGPLOT_LIBS@
pav_LDADD			= $(LDADD) @PGPLOT_LIBS@
pcm_LDADD			= $(LDADD) @PGPLOT_LIBS@ -lpthread
pcm_chooser_LDADD		= $(LDADD) @PGPLOT_LIBS@
pcvar_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrband_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrplot_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrwt_LDADD			= $(LDADD) @PGPLOT_LIBS@
pvt_LDADD			= $(LDADD) @PGPLOT_LIBS@
rmfit_LDADD			= $(LDADD) @PGPLOT_LIBS@
bin_clean_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrqft_LDADD			= $(LDADD) @PGPLOT_LIBS@
plot_baseline_LDADD		= $(LDADD) @PGPLOT_LIBS@
pdmp_LDADD		        = $(LDADD) @PGPLOT_LIBS@
psrfi_LDADD			= $(LDADD) @PGPLOT_LIBS@
spa_LDADD			= $(LDADD) @PGPLOT_LIBS@
tsys_LDADD			= $(LDADD) @PGPLOT_LIBS@
paas_LDADD			= $(LDADD) @PGPLOT_LIBS@
paf_LDADD			= $(LDADD) @PGPLOT_LIBS@
pfixbaseline_LDADD		= $(LDADD) @PGPLOT_LIBS@
psrchive_template_LDADD		= $(LDADD) @PGPLOT_LIBS@
pcmdiff_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrds_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrdiff_LDADD                   = $(LDADD) @PGPLOT_LIBS@

if HAVE_CFITSIO

bin_PROGRAMS += pad plot_acf

pad_SOURCES 			= pad.C
plot_acf_SOURCES		= plot_acf.C
pad_LDADD 			= $(LDADD) @PGPLOT_LIBS@
plot_acf_LDADD			= $(LDADD) @PGPLOT_LIBS@

endif #CFITSIO

endif #PGPLOT

# Config.C must be the last source code in the list
libpsrchive_la_SOURCES += Config.C

##############################################################################
#
# Rule to install doxygen-generated documentation in $HOME/public_html/psrchive
#

EXPORT := ${HOME}/public_html/psrchive

export:
	doxygen
	rm -rf $(EXPORT)
	mv doc/html $(EXPORT)
	chmod -R a+r $(EXPORT)
	chmod a+x $(EXPORT)


#############################################################################
#

LDADD = libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
        $(top_builddir)/Util/libpsrutil.la \
	$(top_builddir)/third/libpsrthird.la

libpsrchive_la_LIBADD += @FFT_LIBS@ @CFITSIO_LIBS@ @FLIBS@

include $(top_srcdir)/config/Makefile.local

# AM_CPPFLAGS += @PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @MPI_CFLAGS@

