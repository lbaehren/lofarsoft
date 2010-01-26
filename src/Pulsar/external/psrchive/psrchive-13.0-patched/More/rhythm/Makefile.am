
bin_PROGRAMS = rhythm

rhythm_SOURCES = rhythm.C menubar.C toolbar.C command_line.C \
	Options.C toaPlot.C psrParams.C psrParameter.C \
	find_standard.C qt_psrParameter.C qt_psrephem.C \
	qt_editParams.C qt_toglParams.C qt_fileParams.C 

EXTRA_DIST = rhythm.h Options.h toaPlot.h psrParams.h psrParameter.h \
	find_standard.h qt_psrParameter.h qt_psrephem.h \
	qt_editParams.h qt_toglParams.h qt_fileParams.h pulsar.xpm 

CLEANFILES = moc_rhythm.C moc_Options.C moc_toaPlot.C \
	moc_qt_psrephem.C moc_qt_editParams.C \
	moc_qt_toglParams.C moc_qt_fileParams.C

nodist_rhythm_SOURCES = $(CLEANFILES)

#############################################################################
#

INCLUDES = -I$(top_builddir)/local_include @QTDRIV_CFLAGS@

LDADD = $(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la @QTDRIV_LIBS@

include $(top_srcdir)/config/Makefile.Qt


