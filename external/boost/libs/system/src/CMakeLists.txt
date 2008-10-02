
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_system_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_system
  ${libboost_system_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_system
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
