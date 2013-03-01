
noinst_LTLIBRARIES = libChecks.la

nobase_dist_data_DATA = fluxcal.on fluxcal.off \
	Parkes/receiver.log Parkes/MULT_1.rcvr \
	Parkes/H-OH.rcvr Parkes/1050CM.rcvr Parkes/5010CM.rcvr

nobase_include_HEADERS = \
        Pulsar/CalSource.h \
	Pulsar/Dedispersed.h \
	Pulsar/DeFaradayed.h \
	Pulsar/SetReceiver.h

libChecks_la_SOURCES = Check_registry.C \
	CalSource.C \
	Dedispersed.C \
	DeFaradayed.C \
	SetReceiver.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

