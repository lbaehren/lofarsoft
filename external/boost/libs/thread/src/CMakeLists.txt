
## -----------------------------------------------------------------------------
## Additional required components

#find_path (HAVE_MULTIPROCESSING_H Multiprocessing.h
#  PATHS /usr/include /usr/local/include /Developer/Headers/FlatCarbon
#  )

## -----------------------------------------------------------------------------
## Source files to build the library

file (GLOB libboost_thread_sources 
#  *.cpp
  pthread/*.cpp
  )

## -----------------------------------------------------------------------------
## Compiler instructions

add_library (boost_thread
  ${libboost_thread_sources}
  )

set_target_properties (boost_thread
  PROPERTIES
  LINK_FLAGS "-fPIC"
  )
  
## -----------------------------------------------------------------------------
## Installation

install (TARGETS boost_thread
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
