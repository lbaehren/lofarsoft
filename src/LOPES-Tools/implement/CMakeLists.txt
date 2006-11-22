PROJECT (LOPES-Tools)

SUBDIRS (
  Calibration
  Data
  Functionals
  IO
  Utilities
  )

## -----------------------------------------------------------------------------
## Build the LopesTools library

INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindBlitz.cmake)
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindBoost.cmake)
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindCASA.cmake)
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindCFITSIO.cmake)
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindCPGPLOT.cmake)
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindROOT.cmake)

## Summary of main search results

message (STATUS "HAVE_CASA  = ${HAVE_CASA}")
message (STATUS "HAVE_BLITZ = ${HAVE_BLITZ}")

## -----------------------------------------------------------------------------

INCLUDE_DIRECTORIES(
  .
  ${CASA_INCLUDE_DIR}
  ${CFITSIO_INCLUDE_DIR}
  ${CPGPLOT_INCLUDE_DIR}
  ${Blitz_INCLUDE_DIR}
  )

if (HAVE_CASA)
  add_definitions (-DHAVE_CASA)
endif (HAVE_CASA)

## -----------------------------------------------------------------------------

ADD_LIBRARY (lopestools
  Calibration/AntennaGain.cc
  Calibration/AverageGain.cc
  Calibration/BaselineGain.cc
  Calibration/CalTableCreator.cc
#  Calibration/CalTableInterpolater.cc
  Calibration/CalTableReader.cc
#  Calibration/CalTableWriter.cc
#  Calibration/PhaseCalibration.cc
#  Calibration/PhaseCalibrationPlugin.cc
#  Calibration/PluginBase.cc
  Calibration/RFIMitigation.cc
#  Calibration/RFIMitigationPlugin.cc
  Data/Data.cc
  Data/DynamicSpectrum.cc
  Data/UnitPrefix.cc
  Display/PGPlot.cc
  Functionals/Hermite1D.cc
  Functionals/Shapelets1D.cc
  Functionals/Shapelets2D.cc
  Functionals/ShapeletsModel1D.cc
  Functionals/StatisticsFilter.cc
  IO/DataIO.cc
  IO/DataIterator.cc
  IO/LogfileName.cc
  Math/Algebra.cc
  Math/ArrayMath.cc
  Math/MathAlgebra.cc
  Math/MathFFT.cc
  Math/MathGeometry.cc
  Math/ThreeVector.cc
  Math/VectorConversion.cc
  Utilities/ProgressBar.cc
  Utilities/Timestamp.cc
  Utilities/TestArray.cc
  )
