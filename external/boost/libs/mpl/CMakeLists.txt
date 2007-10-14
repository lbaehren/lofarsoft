
## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (book)
add_subdirectory (doc)
add_subdirectory (example)
add_subdirectory (test)

## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_mpl_sources 
  test/*.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_definitions (
  -fPIC
  )

add_library (boost_mpl
  ${libboost_mpl_sources}
  )

## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_mpl
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
