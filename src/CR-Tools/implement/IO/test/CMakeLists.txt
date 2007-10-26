##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 392 2007-06-13 10:38:12Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tApplyInstrumentEffects
  tDataBlockSettings
  tDataIterator
  tDataReader
  tFFTServer
  tgenTVSpec
  tImportAntenna
  tImportSimulation
  tFalckeGorhamGalacticNoise
  tLogfileName
  tsimulation2event
  tSimulation2fftMatrix
  ttoLOPESgrid
  tTSmatrix2Event
  )

if (HAVE_HDF5)
  list (APPEND tests tH5ImportConfig)
endif (HAVE_HDF5)

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
