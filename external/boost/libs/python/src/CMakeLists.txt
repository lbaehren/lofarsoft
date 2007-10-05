
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_python_sources 
  *.cpp
  converter/*.cpp
  object/*.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_library (boost_python
  ${libboost_python_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_python
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
