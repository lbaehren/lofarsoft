
SUBDIRS = njkk08 .

noinst_PROGRAMS = example
example_SOURCES = example.C

bin_SCRIPTS = psrhelp

EXTRA_DIST = psrhelp

bin_PROGRAMS = pam pat paz psradd psrsh psrstat psrmodel psrwt \
	fluxcal pac pcm pdv rmfit sesdiv psrstability

pam_SOURCES = pam.C

pat_SOURCES = pat.C
pat_LDADD = $(LDADD) @PTHREAD_LIBS@

paz_SOURCES = paz.C zap_periodic_spikes.C

psradd_SOURCES = psradd.C

psrsh_SOURCES = psrsh.C
psrsh_LDADD = $(LDADD) @READLINE_LIBS@

psrstability_SOURCES = psrstability.C

psrstat_SOURCES = psrstat.C

psrmodel_SOURCES = psrmodel.C

psrwt_SOURCES = psrwt.C

fluxcal_SOURCES = fluxcal.C

pac_SOURCES = pac.C

# Note that pcm uses PGPLOT only if it is available
pcm_SOURCES = pcm.C
pcm_LDADD = $(LDADD) @PGPLOT_LIBS@ @PTHREAD_LIBS@

pdv_SOURCES = pdv.C width.C

# Note that rmfit uses PGPLOT only if it is available
rmfit_SOURCES = rmfit.C width.C

rmfit_LDADD = njkk08/libnjkk08.la $(LDADD) @PGPLOT_LIBS@

sesdiv_SOURCES = sesdiv.C

# ######################################################################
#
# GSL specific code
#
if HAVE_GSL

bin_PROGRAMS += psrsmooth

psrsmooth_SOURCES = psrsmooth.C
psrsmooth_LDADD = $(LDADD) @GSL_LIBS@

endif

# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

bin_PROGRAMS += pdmp psrplot paas pas psrdiff pacv pazi psrzap pav \
	psrsp psrtrash

pdmp_SOURCES = pdmp.C
pdmp_LDADD = $(LDADD) @PGPLOT_LIBS@ @PSRXML_LIBS@

psrplot_SOURCES = psrplot.C
psrplot_LDADD = $(LDADD) @PGPLOT_LIBS@

paas_SOURCES = paas.C
paas_LDADD = $(LDADD) @PGPLOT_LIBS@

pas_SOURCES = pas.C
pas_LDADD = $(LDADD) @PGPLOT_LIBS@

psrdiff_SOURCES = psrdiff.C
psrdiff_LDADD = $(LDADD) @PGPLOT_LIBS@

pacv_SOURCES = pacv.C
pacv_LDADD = $(LDADD) @PGPLOT_LIBS@

pazi_SOURCES = pazi.C
pazi_LDADD = $(LDADD) @PGPLOT_LIBS@

psrzap_SOURCES = psrzap.C
psrzap_LDADD = $(LDADD) @PGPLOT_LIBS@

pav_SOURCES = pav.C PavApp.C PavApp.h
pav_LDADD = $(LDADD) @PGPLOT_LIBS@

psrsp_SOURCES = psrsp.C
psrsp_LDADD = $(LDADD) @PGPLOT_LIBS@

psrtrash_SOURCES = psrtrash.C
psrtrash_LDADD = $(LDADD) @PGPLOT_LIBS@

endif

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @GSL_CFLAGS@ @PSRXML_CFLAGS@

