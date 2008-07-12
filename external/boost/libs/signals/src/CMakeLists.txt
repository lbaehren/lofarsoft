
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_signals_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_signals
  ${libboost_signals_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_signals
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
