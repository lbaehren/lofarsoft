project (libms)

add_library (ms
  MeasurementSets/MeasurementSet.cc
  MeasurementSets/MS1ToMS2Converter.cc
  MeasurementSets/MSAntenna.cc
  MeasurementSets/MSAntennaColumns.cc
#  MeasurementSets/MSAntennaGram.cc
  MeasurementSets/MSAntennaIndex.cc
  MeasurementSets/MSAntennaParse.cc
  MeasurementSets/MSColumns.cc
  MeasurementSets/MSConcat.cc
  MeasurementSets/MSContinuumSubtractor.cc
#  MeasurementSets/MSCorrGram.cc
  MeasurementSets/MSCorrParse.cc
  MeasurementSets/MSDataDescColumns.cc
  MeasurementSets/MSDataDescIndex.cc
  MeasurementSets/MSDataDescription.cc
  MeasurementSets/MSDerivedValues.cc
  MeasurementSets/MSDoppler.cc
  MeasurementSets/MSDopplerColumns.cc
  MeasurementSets/MSDopplerIndex.cc
  MeasurementSets/MSDopplerUtil.cc
  MeasurementSets/MSFeed.cc
  MeasurementSets/MSFeedColumns.cc
  MeasurementSets/MSFeedIndex.cc
  MeasurementSets/MSField.cc
  MeasurementSets/MSFieldColumns.cc
 # MeasurementSets/MSFieldGram.cc
  MeasurementSets/MSFieldIndex.cc
  MeasurementSets/MSFieldParse.cc
  MeasurementSets/MSFlagCmd.cc
  MeasurementSets/MSFlagCmdColumns.cc
  MeasurementSets/MSFlagger.cc
  MeasurementSets/MSFreqOffColumns.cc
  MeasurementSets/MSFreqOffIndex.cc
  MeasurementSets/MSFreqOffset.cc
  MeasurementSets/MSHistory.cc
  MeasurementSets/MSHistoryColumns.cc
  MeasurementSets/MSHistoryHandler.cc
  MeasurementSets/MSIter.cc
  MeasurementSets/MSLister.cc
  MeasurementSets/MSMainColumns.cc
  MeasurementSets/MSObsColumns.cc
  MeasurementSets/MSObservation.cc
  MeasurementSets/MSObsIndex.cc
  MeasurementSets/MSParse.cc
  MeasurementSets/MSPointing.cc
  MeasurementSets/MSPointingColumns.cc
  MeasurementSets/MSPointingIndex.cc
  MeasurementSets/MSPolarization.cc
  MeasurementSets/MSPolColumns.cc
  MeasurementSets/MSPolIndex.cc
  MeasurementSets/MSProcessor.cc
  MeasurementSets/MSProcessorColumns.cc
  MeasurementSets/MSRange.cc
  MeasurementSets/MSReader.cc
#  MeasurementSets/MSScanGram.cc
  MeasurementSets/MSScanParse.cc
  MeasurementSets/MSSelection.cc
  MeasurementSets/MSSelectionError.cc
  MeasurementSets/MSSelectionKeywords.cc
  MeasurementSets/MSSelector.cc
  MeasurementSets/MSSelUtil2.cc
  MeasurementSets/MSSelUtil.cc
  MeasurementSets/MSSimulator.cc
  MeasurementSets/MSSource.cc
  MeasurementSets/MSSourceColumns.cc
  MeasurementSets/MSSourceIndex.cc
  MeasurementSets/MSSpectralWindow.cc
#  MeasurementSets/MSSpwGram.cc
  MeasurementSets/MSSpwIndex.cc
  MeasurementSets/MSSpWindowColumns.cc
  MeasurementSets/MSSpWindowIndex.cc
  MeasurementSets/MSSpwParse.cc
  MeasurementSets/MSState.cc
  MeasurementSets/MSStateColumns.cc
  MeasurementSets/MSSummary.cc
  MeasurementSets/MSSysCal.cc
  MeasurementSets/MSSysCalColumns.cc
  MeasurementSets/MSSysCalIndex.cc
  MeasurementSets/MSTable2.cc
  MeasurementSets/MSTable.cc
  MeasurementSets/MSTableImpl.cc
  MeasurementSets/MSTableIndex.cc
  MeasurementSets/MSTileLayout.cc
#  MeasurementSets/MSTimeGram.cc
  MeasurementSets/MSTimeParse.cc
#  MeasurementSets/MSUvDistGram.cc
  MeasurementSets/MSUvDistParse.cc
  MeasurementSets/MSValidIds.cc
  MeasurementSets/MSWeather.cc
  MeasurementSets/MSWeatherColumns.cc
  MeasurementSets/MSWeatherIndex.cc
  MeasurementSets/NewMSSimulator.cc
  MeasurementSets/StokesConverter.cc
## -- ms/implement/MSPlot ------------------------
  MSPlot/MsPlot.cc
## -- template instanatiation --------------------
  ../../templates/ms/Array.cc
  ../../templates/ms/CountedPtr.cc
  ../../templates/ms/MaskedArray.cc
  ../../templates/ms/MsPlot.cc
  ../../templates/ms/MSTable.cc
  ../../templates/ms/Vector.cc
  )
