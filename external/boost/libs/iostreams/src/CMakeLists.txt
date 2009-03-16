
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_iostreams_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_iostreams
  ${libboost_iostreams_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_iostreams
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
