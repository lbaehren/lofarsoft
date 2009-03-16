
## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_iostreams_sources 
  *.cpp
  )

if (NOT HAVE_BZLIB_H)
  list (REMOVE_ITEM libboost_iostreams_sources ${BOOST_SOURCE_DIR}/libs/iostreams/src/bzip2.cpp)
endif (NOT HAVE_BZLIB_H)

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
