
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_test_sources *.cpp)

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_unit_test_framework
  ${libboost_test_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_unit_test_framework
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
