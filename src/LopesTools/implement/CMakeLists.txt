PROJECT (LopesTools)

SUBDIRS (Data Functionals IO Utilities)

## -----------------------------------------------------------------------------
## Build the LopesTools library

INCLUDE (${LopesTools_SOURCE_DIR}/../../../devel_common/cmake/FindBoost.cmake)
INCLUDE (${LopesTools_SOURCE_DIR}/../../../devel_common/cmake/FindCASA.cmake)
INCLUDE (${LopesTools_SOURCE_DIR}/../../../devel_common/cmake/FindCFITSIO.cmake)
INCLUDE (${LopesTools_SOURCE_DIR}/../../../devel_common/cmake/FindCPGPLOT.cmake)

INCLUDE_DIRECTORIES(. ${CFITSIO_INCLUDE_DIR} ${CPGPLOT_INCLUDE_DIR})

ADD_LIBRARY (lopestools
	Data/Data.cc
	Data/DynamicSpectrum.cc
	Display/PGPlot.cc
	Functionals/Hermite1D.cc
	Functionals/Shapelets1D.cc
	Functionals/Shapelets2D.cc
	Functionals/ShapeletsModel1D.cc
	IO/DataIO.cc
	IO/DataIterator.cc
	Math/ThreeVector.cc
	Utilities/ProgressBar.cc
	Utilities/Timestamp.cc)
