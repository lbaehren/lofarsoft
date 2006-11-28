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
INCLUDE (${LOPES-Tools_SOURCE_DIR}/../../../devel_common/cmake/FindWCS.cmake)

## -----------------------------------------------------------------------------

INCLUDE_DIRECTORIES(
  .
  ${CASA_INCLUDE_DIR}
  ${CFITSIO_INCLUDE_DIR}
  ${CPGPLOT_INCLUDE_DIR}
  ${Blitz_INCLUDE_DIR}
  ${WCS_INCLUDE_DIR}
  )

if (HAVE_CASA)
  add_definitions (-DHAVE_CASA)
endif (HAVE_CASA)

## -----------------------------------------------------------------------------

ADD_LIBRARY (lopestools
## -- Analysis -----------------------------------
  Analysis/ClippingFraction.cc
  Analysis/DataBlockStatistics.cc
#  Analysis/DynamicSpectrum.cc
  Analysis/Masking.cc
  Analysis/Peak.cc
  Analysis/PeakList.cc
  Analysis/PeakSearch.cc
  Analysis/SourceFit.cc
##  -- Calibration -------------------------------
  Calibration/AntennaGain.cc
  Calibration/AverageGain.cc
  Calibration/BaselineGain.cc
  Calibration/CalTableCreator.cc
  Calibration/CalTableInterpolater.cc
  Calibration/CalTableReader.cc
  Calibration/CalTableWriter.cc
  Calibration/PhaseCalibration.cc
  Calibration/PhaseCalibrationPlugin.cc
  Calibration/PluginBase.cc
  Calibration/RFIMitigation.cc
  Calibration/RFIMitigationPlugin.cc
## -- Coordinates --------------------------------
  Coordinates/AxisCoordinates.cc
  Coordinates/AxisIterator.cc
  Coordinates/CoordinateGrid.cc
  Coordinates/GeodeticEllipsoid.cc
  Coordinates/GridCoordinate.cc
  Coordinates/PolarCoordinate.cc
## -- Data ---------------------------------------
  Data/Data.cc
  Data/DynamicSpectrum.cc
  Data/TIM40.cc
  Data/UnitPrefix.cc
## -- Display ------------------------------------
  Display/PGPlot.cc
## -- Functionals --------------------------------
  Functionals/Hermite1D.cc
  Functionals/Shapelets1D.cc
  Functionals/Shapelets2D.cc
  Functionals/ShapeletsModel1D.cc
  Functionals/StatisticsFilter.cc
## -- IO -----------------------------------------
  IO/DataIO.cc
  IO/DataIterator.cc
  IO/LogfileName.cc
## -- LopesBase ----------------------------------
  LopesBase/BasicHeader.cc
  LopesBase/BasicObject.cc
  LopesBase/DataConversion.cc
  LopesBase/DataCoordinfo.cc
  LopesBase/DataFrequency.cc
  LopesBase/ExchangeFormat.cc
  LopesBase/StoredAddObject.cc
  LopesBase/StoredInputObject.cc
  LopesBase/StoredMultiplyObject.cc
  LopesBase/StoredObject.cc
## -- Math ---------------------------------------
  Math/Algebra.cc
  Math/ArrayMath.cc
  Math/MathAlgebra.cc
  Math/MathFFT.cc
  Math/MathFitting.cc
  Math/MathGeometry.cc
  Math/MatrixEigen.cc
  Math/NR.cc
  Math/Statistics.cc
  Math/ThreeVector.cc
  Math/VectorConversion.cc
## -- Utilities ----------------------------------
  Utilities/ProgressBar.cc
  Utilities/Timestamp.cc
  Utilities/TestArray.cc
  )
