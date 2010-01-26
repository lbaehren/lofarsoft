
noinst_LTLIBRARIES = libqtutil.la

include_HEADERS = qt_value.h qt_double.h qtools.h qt_Angle.h qt_MJD.h \
	ConfigurableScrollBar.h RangePolicy.h

libqtutil_la_SOURCES = qt_value.C qt_double.C qtools.C qt_Angle.C qt_MJD.C \
	ConfigurableScrollBar.C RangePolicy.C

MOC_SOURCES = moc_qt_value.C moc_qt_double.C moc_qt_Angle.C moc_qt_MJD.C \
	moc_ConfigurableScrollBar.C

nodist_libqtutil_la_SOURCES = $(MOC_SOURCES)

check_PROGRAMS = qt_double_test qt_Angle_test qt_MJD_test \
	test_ConfigurableScrollBar

qt_double_test_SOURCES = qt_double_test.C
qt_Angle_test_SOURCES = qt_Angle_test.C
qt_MJD_test_SOURCES = qt_MJD_test.C
test_ConfigurableScrollBar_SOURCES = test_ConfigurableScrollBar.C

include $(top_srcdir)/config/Makefile.Qt

#############################################################################
#

LDADD = libqtutil.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la 

libqtutil_la_LIBADD = @QT_LIBS@ 

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @QT_CXXFLAGS@

