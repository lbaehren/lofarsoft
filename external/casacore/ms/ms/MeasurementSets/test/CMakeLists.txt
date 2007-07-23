
## ------------------------------------------------------------------------------
## Test programs for ms/MeasurementSets

SET (tests
  tMSAntennaGram
  tMSColumns
  tMSCorrGram
  tMSDataDescBuffer
  tMSDerivedValues
  tMSFieldBuffer
  tMSFieldGram
  tMSMainBuffer
  tMSPolBuffer
  tMSReader
  tMSScanGram
  tMSSimulator
  tMSSpwGram
  tMSSummary
  tMSTimeGram
  tMSUvDistGram
  tMeasurementSet
  tStokesConverter
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} ms)
ENDFOREACH (src)
