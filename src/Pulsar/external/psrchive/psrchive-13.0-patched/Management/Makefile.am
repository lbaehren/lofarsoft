
TESTS = psrchive_config

bin_PROGRAMS = psrchive_info psrchive_config

psrchive_info_SOURCES   = psrchive_info.C
psrchive_config_SOURCES = psrchive_config.C Config_ensure_linkage.C

bin_SCRIPTS = psrchive

noinst_SCRIPTS = release.csh

dist_bin_SCRIPTS = psrchive_install

dist_data_DATA = psrchive_version

aclocaldir = $(datadir)/aclocal
dist_aclocal_DATA = psrchive.m4

htmldir = $(datadir)/html
dist_html_DATA = psrchive_layout.shtml psrchive_top.shtml psrchive_side.shtml

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

all:
	chmod +x $(noinst_SCRIPTS)

libs include:
	@echo > /dev/null

