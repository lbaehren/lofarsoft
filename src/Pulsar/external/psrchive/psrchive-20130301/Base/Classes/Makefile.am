
noinst_LTLIBRARIES = libClasses.la

nobase_include_HEADERS = \
	Pulsar/psrchive.h \
	Pulsar/Agent.h \
        Pulsar/Archive.h \
        Pulsar/ArchiveExpert.h \
	Pulsar/ArchiveExtension.h \
        Pulsar/ArchiveInterface.h \
	Pulsar/ArchiveMatch.h \
	Pulsar/Application.h \
        Pulsar/BasicArchive.h \
        Pulsar/BasicIntegration.h \
	Pulsar/Calibrator.h \
	Pulsar/CalibratorType.h \
	Pulsar/CalibratorTypeInterface.h \
	Pulsar/CalibratorTypes.h \
	Pulsar/Check.h \
        Pulsar/Config.h \
        Pulsar/Container.h \
	Pulsar/Correction.h \
	Pulsar/DataExtension.h \
	Pulsar/Editor.h \
        Pulsar/ExampleArchive.h \
	Pulsar/FITSAlias.h \
	Pulsar/FourthMoments.h \
        Pulsar/Integration.h \
        Pulsar/IntegrationExpert.h \
        Pulsar/IntegrationExtension.h \
        Pulsar/IntegrationManagerExpert.h \
        Pulsar/IntegrationManager.h \
	Pulsar/IntegrationMeta.h \
        Pulsar/IntegrationTI.h \
	Pulsar/MoreProfiles.h \
        Pulsar/Profile.h \
	Pulsar/ProfileAmps.h \
	Pulsar/ProfileAmpsExpert.h \
	Pulsar/ProfileExtension.h \
        Pulsar/Pulsar.h \
	Pulsar/ThresholdMatch.h \
	Pulsar/UnloadOptions.h \
	Pulsar/Verification.h

libClasses_la_SOURCES = \
        Agent.C \
        Agent_plugin.C \
        Archive.C \
        ArchiveInterface.C \
	ArchiveMatch.C \
	Archive_correct.C \
        Archive_copy.C \
	Archive_extract.C \
        Archive_init_Integration.C \
	Archive_load_Integration.C \
        Archive_load.C \
	Archive_match.C \
        Archive_resize.C \
        Archive_remove_chan.C \
	Archive_verify.C \
        Archive_unload.C \
        Archive_verbose.C \
	Application.C \
        BasicArchive.C \
        BasicIntegration.C \
	Calibrator.C \
	CalibratorType.C \
	CalibratorTypeInterface.C \
	Check.C \
        Config.C \
	Editor.C \
        ExampleArchive.C \
	FITSAlias.C \
	FourthMoments.C \
        Integration.C \
        IntegrationManager.C \
	IntegrationMeta.C \
        Integration_combine.C \
	Integration_insert.C \
	Integration_remove.C \
        Integration_mixable.C \
        Integration_resize.C \
        IntegrationTI.C \
	MoreProfiles.C \
        Profile_average.C \
	Profile_rotate.C \
        Profile.C \
	ProfileAmps.C \
	Pulsar.C \
	ThresholdMatch.C \
	UnloadOptions.C

TESTS = test_Config

check_PROGRAMS = $(TESTS)

test_Config_SOURCES = test_Config.C

#############################################################################
#

LDADD = libClasses.la \
	$(top_builddir)/Util/libpsrutil.la

INCLUDES = -I$(top_srcdir)/Base/Extensions

include $(top_srcdir)/config/Makefile.include

