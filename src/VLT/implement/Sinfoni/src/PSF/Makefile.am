INSTHDRS = \
	ApertureToFriedRatio.h \
	ControlMatrix.h \
	InputData.h \
	InputDataRep.h \
	InputDataRepAscii.h \
	InputDataRepFits.h \
	ModalCoordinates.h \
	ModalCoordResidualPhase.h \
	NoiseTransfer.h \
	OTFHighOrderPhaseContrib.h \
	OTFMirrorCompContrib.h \
	OTFTelescope.h \
	PointSpreadFunction.h \
	PropagatedMeasurementError.h \
	PSFConfig.h \
	QualityMetrics.h \
	RemainingError.h \
	TransformedNollMatrix.h \
	VLTPupil.h \
	VoltageToZernike.h

NOINSTHDRS =

TCCHDRS = 

pkginclude_HEADERS = $(INSTHDRS) $(TCCHDRS)

noinst_HEADERS = $(NOINSTHDRS)

DOCHDRS = $(INSTHDRS) $(NOINSTHDRS)

lib_LTLIBRARIES = libpsf.la

libpsf_la_SOURCES = \
	ApertureToFriedRatio.cc \
	ControlMatrix.cc \
	InputData.cc \
	InputDataRepAscii.cc \
	InputDataRepFits.cc \
	ModalCoordinates.cc \
	ModalCoordResidualPhase.cc \
	NoiseTransfer.cc \
	OTFHighOrderPhaseContrib.cc \
	OTFMirrorCompContrib.cc \
	OTFTelescope.cc \
	PointSpreadFunction.cc \
	PropagatedMeasurementError.cc \
	QualityMetrics.cc \
	RemainingError.cc \
	TransformedNollMatrix.cc \
	VLTPupil.cc \
	VoltageToZernike.cc

libpsf_la_LIBADD = ../libsinfoni.la

CXXFLAGS	+= -Wno-comment

include $(lofar_sharedir)/Makefile.common
