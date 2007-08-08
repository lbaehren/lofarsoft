##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  ascii_export_baseline
  ascii_export_spectra
  tAverageGain
  tBaselineGain
  tCalTableInterpolater
  tCalTables
  tPhaseCalibration
  tPhaseCalibrationPlugin
  tPluginBase
  tRFIMitigation
  tTVCalibrationPlugin
  )

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
endforeach (src)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
