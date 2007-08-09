##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 392 2007-06-13 10:38:12Z baehren                       $
##------------------------------------------------------------------------------

include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tDataBlockSettings
  tDataIterator
  tDataReader
  tDataReaderTools
  tFFTServer
  tLogfileName
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
