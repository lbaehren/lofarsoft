
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_serialization_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_serialization
  ${libboost_serialization_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_serialization
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
