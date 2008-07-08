
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_wave_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_wave
  ${libboost_wave_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_wave
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
