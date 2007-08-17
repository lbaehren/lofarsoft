
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
  TARGET_LINK_LIBRARIES (${src} ms measures tables scimath casa)
  ## testing 
  add_test (${src} ${src})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
