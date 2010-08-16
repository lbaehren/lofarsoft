
bin_PROGRAMS = psredit psrtxt psrconv psrsplit vip vap

psredit_SOURCES		= psredit.C
psrtxt_SOURCES		= psrtxt.C
psrconv_SOURCES		= psrconv.C
psrsplit_SOURCES	= psrsplit.C
vip_SOURCES		= vip.C
vap_SOURCES		= vap.C

check_PROGRAMS = test_threads

test_threads_SOURCES	= test_threads.C

#############################################################################

INCLUDES = -I$(top_builddir)/local_include

LDADD = $(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

