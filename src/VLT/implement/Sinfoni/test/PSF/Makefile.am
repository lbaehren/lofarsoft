check_PROGRAMS			= \
				  tNoiseTransfer \
				  tControlMatrix \
				  tTransformedNollMatrix \
				  tOTFTelescope \
				  tInputData \
				  tPropagatedMeasurementError \
				  tApertureToFriedRatio \
				  tModalCoordResidualPhase \
				  tOTFHighOrderPhaseContrib \
				  tOTFMirrorCompContrib \
				  tPointSpreadFunction \
				  tQualityMetrics

TESTS				= \
				  tNoiseTransfer.sh \
				  tControlMatrix_test.sh \
				  tTransformedNollMatrix_test.sh \
				  tOTFTelescope_test.sh \
				  tInputData_test.sh \
				  tPropagatedMeasurementError_test.sh \
				  tApertureToFriedRatio_test.sh \
				  tModalCoordResidualPhase_test.sh \
				  tOTFHighOrderPhaseContrib_test.sh \
				  tOTFMirrorCompContrib_test.sh \
				  tPointSpreadFunction_test.sh \
				  tQualityMetrics_test.sh

tNoiseTransfer_SOURCES		= tNoiseTransfer.cc
tNoiseTransfer_LDADD		= ../../src/PSF/libpsf.la
tNoiseTransfer_DEPENDENCIES	= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tControlMatrix_SOURCES		= tControlMatrix.cc
tControlMatrix_LDADD		= ../../src/PSF/libpsf.la
tControlMatrix_DEPENDENCIES	= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tTransformedNollMatrix_SOURCES	= tTransformedNollMatrix.cc
tTransformedNollMatrix_LDADD	= ../../src/PSF/libpsf.la
tTransformedNollMatrix_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tOTFTelescope_SOURCES		= tOTFTelescope.cc
tOTFTelescope_LDADD		= ../../src/PSF/libpsf.la
tOTFTelescope_DEPENDENCIES	= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tInputData_SOURCES		= tInputData.cc
tInputData_LDADD		= ../../src/PSF/libpsf.la
tInputData_DEPENDENCIES		= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tPropagatedMeasurementError_SOURCES = tPropagatedMeasurementError.cc
tPropagatedMeasurementError_LDADD = ../../src/PSF/libpsf.la
tPropagatedMeasurementError_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tApertureToFriedRatio_SOURCES	= tApertureToFriedRatio.cc
tApertureToFriedRatio_LDADD	= ../../src/PSF/libpsf.la
tApertureToFriedRatio_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tModalCoordResidualPhase_SOURCES = tModalCoordResidualPhase.cc
tModalCoordResidualPhase_LDADD	= ../../src/PSF/libpsf.la
tModalCoordResidualPhase_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tOTFHighOrderPhaseContrib_SOURCES = tOTFHighOrderPhaseContrib.cc
tOTFHighOrderPhaseContrib_LDADD	= ../../src/PSF/libpsf.la
tOTFHighOrderPhaseContrib_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tOTFMirrorCompContrib_SOURCES	= tOTFMirrorCompContrib.cc
tOTFMirrorCompContrib_LDADD	= ../../src/PSF/libpsf.la
tOTFMirrorCompContrib_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tPointSpreadFunction_SOURCES	= tPointSpreadFunction.cc
tPointSpreadFunction_LDADD	= ../../src/PSF/libpsf.la
tPointSpreadFunction_DEPENDENCIES = ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

tQualityMetrics_SOURCES		= tQualityMetrics.cc
tQualityMetrics_LDADD		= ../../src/PSF/libpsf.la
tQualityMetrics_DEPENDENCIES	= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)


CXXFLAGS			+= -Wno-comment

include $(lofar_sharedir)/Makefile.common
