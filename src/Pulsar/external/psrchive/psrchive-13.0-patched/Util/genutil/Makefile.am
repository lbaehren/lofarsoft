
noinst_LTLIBRARIES = libgenutil.la

include_HEADERS = \
        Angle.h \
	BatchQueue.h \
        Brent.h \
	BoxMuller.h \
        Cartesian.h \
	CommandLine.h \
        CommandParser.h \
        compressed_io.h \
	coord.h \
        coord_parse.h \
	Directional.h \
        dirutil.h \
	evaluate.h \
	ExponentialDistribution.h \
	extutil.h \
        fcomp.h \
	format_it.h \
        fsleep.h \
	Horizon.h \
        ieee.h \
        ierf.h \
        JenetAnderson98A5.h \
        JenetAnderson98.h \
	load_factory.h \
	LogFile.h \
        machine_endian.h \
	malloc16.h \
	Meridian.h \
        MJD.h \
	Mount.h \
	FITSUTC.h \
	NormalDistribution.h \
        orbital.h \
	pairutil.h \
        Physical.h \
	Probability.h \
        Plugin.h \
        RealTimer.h \
        RegularExpression.h \
        sky_coord.h \
	StraightLine.h \
        strutil.h \
        templates.h \
	TemporaryFile.h \
	ThreadContext.h \
	ThreadStream.h \
        Types.h \
        typeutil.h \
        utc.h \
	VanVleck.h \
	VirtualMemory.h \
	table_stream.h


libgenutil_la_SOURCES = \
        Angle.C \
        angleconv.c \
	BatchQueue.C \
	BoxMuller.C \
        Cartesian.C \
	CommandLine.C \
        CommandParser.C \
        companion_mass.c \
        completion.C \
        compressed_io.C \
        coord_parse.c \
	copy.C \
	Directional.C \
        dirglob.C \
	dirglobtree.C \
        dirutil.c \
        eccentric_anomaly.c \
        endian.c \
        errscale.c \
	expand.C \
	evaluate.C \
	ExponentialDistribution.C \
        fcomp.C \
	filesize.c \
	format_it.C \
        fsleep.c \
	Horizon.C \
        ieee.c \
        ierf.c \
        JenetAnderson98A5.C \
        JenetAnderson98.C \
	LogFile.C \
	malloc16.c \
	Meridian.C \
        MJD.C \
	Mount.C \
	FITSUTC.C \
	NormalDistribution.C \
        Physical.C \
	printable.C \
	Probability.C \
        Plugin.C \
        RealTimer.C \
        RegularExpression.C \
	replace_extension.C \
        sky_coord.C \
        sla_MJD.C \
        sla_sky_coord.C \
	smooth.f \
        stringload.C \
        stringprintf.C \
        stringtok.C \
        strutil.C \
	TemporaryFile.C \
	ThreadContext.C \
	ThreadStream.C \
	time_string.C \
        Types.C \
        utc.c \
	VanVleck.C \
	VirtualMemory.C \
	table_stream.C

if HAVE_PGPLOT
include_HEADERS += JenetAnderson98Plot.h
libgenutil_la_SOURCES += JenetAnderson98Plot.C
endif

if HAVE_GSL
include_HEADERS += PolynomialFit.h Interpolation.h dwt_undec.h
libgenutil_la_SOURCES += PolynomialFit.C Interpolation.C dwt_undec.c
endif

libgenutil_la_LIBADD = @READLINE_LIBS@ @GSL_LIBS@

bin_PROGRAMS = getMJD

getMJD_SOURCES = getMJD.C

TESTS = test_evaluate test_coord test_Cartesian test_MJD test_ja98	\
	test_endian test_sum test_NormalDistribution			\
	test_TemporaryFile test_moment2 test_MJD_ostream test_sky_coord	\
	test_exponential test_StraightLine test_ThreadStream		\
	test_Horizon test_LogFile

check_PROGRAMS = $(TESTS) test_CommandLine test_CommandParser \
	test_Angle test_expand test_VirtualMemory

test_evaluate_SOURCES           = test_evaluate.C
test_Cartesian_SOURCES		= test_Cartesian.C
test_MJD_SOURCES		= test_MJD.C
test_Angle_SOURCES		= test_Angle.C
test_Angle_LDADD		= $(LDADD) @FLIBS@
test_CommandLine_SOURCES	= test_CommandLine.C
test_CommandParser_SOURCES      = test_CommandParser.C
test_CommandParser_LDADD	= $(LDADD) @READLINE_LIBS@
test_ja98_SOURCES		= test_ja98.C
test_sum_SOURCES		= test_sum.C
test_TemporaryFile_SOURCES	= test_TemporaryFile.C
test_moment2_SOURCES		= test_moment2.C
test_exponential_SOURCES	= test_exponential.C
test_MJD_ostream_SOURCES	= test_MJD_ostream.C
test_sky_coord_SOURCES		= test_sky_coord.C
test_expand_SOURCES		= test_expand.C
test_NormalDistribution_SOURCES = test_NormalDistribution.C
test_StraightLine_SOURCES	= test_StraightLine.C
test_ThreadStream_SOURCES	= test_ThreadStream.C
test_Horizon_SOURCES		= test_Horizon.C
test_VirtualMemory_SOURCES	= test_VirtualMemory.C
test_LogFile_SOURCES		= test_LogFile.C

#############################################################################
#

LDADD = libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @GSL_CFLAGS@

