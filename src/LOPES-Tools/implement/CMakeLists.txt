PROJECT (LOPES-Tools)

## -----------------------------------------------------------------------------
## libcasa

SET (lopes_modules
  Analysis
  Beamforming
  Calibration
  Coordinates
  Data
  Display
  Functionals
  IO
  LopesBase
  Math
  Observation
  Skymap
  Testing
  Utilities
  )

## collect the source files in the modules

foreach (lopes_module ${lopes_modules})
  FILE (GLOB lopes_${lopes_module} ${lopes_module}/*.cc)
  LIST(APPEND lopes_sources ${lopes_${lopes_module}})
endforeach (lopes_module)

SUBDIRS (
  Analysis
  Calibration
  Data
  Functionals
  IO
  Testing/test
  Utilities/test
  )

## -----------------------------------------------------------------------------
## Build the LopesTools library

ADD_LIBRARY (lopestools
  templates.cc
## -- Analysis ---------------------------------------------
#  Analysis/ClippingFraction.cc
#  Analysis/DataBlockStatistics.cc
#  Analysis/DynamicSpectrum.cc
#  Analysis/Peak.cc
#  Analysis/PeakList.cc
#  Analysis/PeakSearch.cc
#  Analysis/SourceFit.cc
## -- Beamforming ------------------------------------------
#  Beamforming/Beamformer.cc
#  Beamforming/ccBeam.cc
#  Beamforming/GeomDelay.cc
#  Beamforming/GeomPhase.cc
#  Beamforming/Phasing.cc
#  Beamforming/XBeam.cc
##  -- Calibration -----------------------------------------
#  Calibration/AntennaGain.cc
#  Calibration/AverageGain.cc
#  Calibration/BaselineGain.cc
#  Calibration/CalTableCreator.cc
#  Calibration/CalTableInterpolater.cc
#  Calibration/CalTableReader.cc
#  Calibration/CalTableWriter.cc
#  Calibration/PhaseCalibration.cc
#  Calibration/PhaseCalibrationPlugin.cc
#  Calibration/PluginBase.cc
#  Calibration/RFIMitigation.cc
#  Calibration/RFIMitigationPlugin.cc
## -- Coordinates ------------------------------------------
#  Coordinates/AxisCoordinates.cc
#  Coordinates/AxisIterator.cc
#  Coordinates/CoordinateGrid.cc
#  Coordinates/GeodeticEllipsoid.cc
#  Coordinates/GridCoordinate.cc
#  Coordinates/PolarCoordinate.cc
## -- Data -------------------------------------------------
  Data/Data.cc
  Data/DataCC.cc
  Data/DataFlag.cc
  Data/DynamicSpectrum.cc
  Data/ITSBeam.cc
  Data/ITSCapture.cc
  Data/ITSCorrelation.cc
  Data/ITSMetadata.cc
  Data/LopesEvent.cc
#  Data/MSFiller.cc
  Data/NDABeam.cc
  Data/nuMoon.cc
  Data/NuMoonHeader.cc
  Data/TIM40.cc
  Data/TIM40Header.cc
  Data/UnitPrefix.cc
## -- Display ----------------------------------------------
  Display/PGPlot.cc
## -- Functionals ------------------------------------------
  ${lopes_Functionals}
## -- IO ---------------------------------------------------
  IO/DataBlockSettings.cc
  IO/DataIO.cc
  IO/DataIterator.cc
#  IO/DataReader.cc
#  IO/DataReaderTools.cc
  IO/ImportAntenna.cc
  IO/LogfileName.cc
## -- LopesBase --------------------------------------------
  LopesBase/BasicHeader.cc
  LopesBase/BasicObject.cc
  LopesBase/DataConversion.cc
  LopesBase/DataCoordinfo.cc
  LopesBase/DataFrequency.cc
  LopesBase/ExchangeFormat.cc
#  LopesBase/StoredAddObject.cc
#  LopesBase/StoredInputObject.cc
#  LopesBase/StoredMultiplyObject.cc
#  LopesBase/StoredObject.cc
## -- Math -------------------------------------------------
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
## -- Observation ------------------------------------------
  Observation/AntennaLayout.cc
  Observation/ObservationData.cc
  Observation/ObservationFrame.cc
  Observation/ObservationMeta.cc
## -- Skymap -----------------------------------------------
  Skymap/Skymap.cc
  Skymap/SkymapGrid.cc
#  Skymap/Skymapper.cc
  Skymap/SkymapperTools.cc
  Skymap/SkymapQuantity.cc
## -- Testing ----------------------------------------------
  Testing/Templates.cc
## -- Utilities --------------------------------------------
  ${lopes_Utilities}
## -- template instantiation -------------------------------
#  templates.cc
  )
