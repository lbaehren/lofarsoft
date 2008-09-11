
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_date_time_sources 
  gregorian/*.cpp
  posix_time/*.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_date_time
  ${libboost_date_time_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_date_time
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
