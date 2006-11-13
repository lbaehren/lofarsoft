check_PROGRAMS		= \
			  matvecTest \
			  svdTest \
			  testMath

matvecTest_SOURCES	= matvecTest.cc
matvecTest_LDADD	= ../src/libaomath.la
matvecTest_DEPENDENCIES	= ../src/libaomath.la $(LOFAR_DEPEND)

svdTest_SOURCES		= svdTest.cc
svdTest_LDADD		= ../src/libaomath.la
svdTest_DEPENDENCIES	= ../src/libaomath.la $(LOFAR_DEPEND)

testMath_SOURCES	= testMath.cc
testMath_LDADD		= ../src/libaomath.la
testMath_DEPENDENCIES	= ../src/libaomath.la $(LOFAR_DEPEND)


include $(top_srcdir)/Makefile.common
