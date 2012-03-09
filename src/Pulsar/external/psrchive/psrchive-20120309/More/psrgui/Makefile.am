
noinst_LTLIBRARIES = libpsrgui.la

nobase_include_HEADERS = PulsarGUI.h plotItem.h interfacePanel.h \
	Pulsar/PlotWindow.h Pulsar/UsingXSERVE.h Pulsar/UsingQTDRIV.h \
	Pulsar/InterfaceDialog.h

libpsrgui_la_SOURCES = PulsarGUI.C plotItem.C interfacePanel.C \
	PlotWindow.C UsingXSERVE.C InterfaceDialog.C

if HAVE_QTDRIV
libpsrgui_la_SOURCES += UsingQTDRIV.C
endif

MOC_SOURCES = moc_PulsarGUI.C moc_plotItem.C moc_interfacePanel.C \
	moc_InterfaceDialog.C

nodist_libpsrgui_la_SOURCES = $(MOC_SOURCES)

bin_PROGRAMS = psrgui

psrgui_SOURCES = psrgui.C


#############################################################################
#

INCLUDES = -I$(top_builddir)/local_include

LDADD = libpsrgui.la \
        $(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la \
        @QTDRIV_LIBS@ @QT_LIBS@ @PGPLOT_LIBS@ 

include $(top_srcdir)/config/Makefile.Qt
include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @QT_CXXFLAGS@

