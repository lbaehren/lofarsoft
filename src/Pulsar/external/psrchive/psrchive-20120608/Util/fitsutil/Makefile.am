
noinst_LTLIBRARIES = libfitsutil.la

include_HEADERS = fitsutil.h FITSError.h psrfitsio.h

libfitsutil_la_SOURCES = FITSError.C fitsutil.C psrfitsio.C \
	fits_version_check.C

TESTS = test_version
check_PROGRAMS = $(TESTS)
test_version_SOURCES = test_version.C

#############################################################################
#

libfitsutil_la_LIBADD = @CFITSIO_LIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @CFITSIO_CFLAGS@

LDADD = libfitsutil.la \
	$(top_builddir)/Util/units/libunits.la
