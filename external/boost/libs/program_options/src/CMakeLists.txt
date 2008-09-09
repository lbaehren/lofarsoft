
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_program_options_sources 
  *.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_program_options
  ${libboost_program_options_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_program_options
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
