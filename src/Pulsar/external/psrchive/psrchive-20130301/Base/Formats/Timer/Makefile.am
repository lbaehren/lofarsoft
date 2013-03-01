
# ############################################################################
#
# The TimerArchive psrchive plugin
#
# ############################################################################

noinst_LTLIBRARIES = libtimer.la

nobase_include_HEADERS = mini++.h timer++.h timer.h mini.h band.h	\
	timer_supp.h							\
	Pulsar/TimerArchive.h Pulsar/TimerIntegration.h			\
	Pulsar/BasebandArchive.h Pulsar/baseband_header.h		\
	Pulsar/ScintArchive.h

libtimer_la_SOURCES = BasebandArchive.C ScintArchive.C TimerArchive.C \
	TimerArchive_Receiver.C ScintPowerEstimator.C ScintPowerEstimator.h \
	TimerArchive_load.C TimerArchive_unload.C TimerArchive_extensions.C \
	TimerIntegration.C TimerIntegration_load.C TimerIntegration_unload.C \
	TimerProfile_load.C TimerProfile_unload.C TimerAgent.C mini++.C \
	timer++.C convert_endian.c convert_endian.h

check_PROGRAMS = baseband_header_size timer_size
TESTS = $(check_PROGRAMS)

baseband_header_size_SOURCES = baseband_header_size.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

