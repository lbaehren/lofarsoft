
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_filesystem_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_filesystem
  ${libboost_filesystem_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_filesystem
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
