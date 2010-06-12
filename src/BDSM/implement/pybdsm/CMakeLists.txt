## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

## List of source files

file (GLOB cbdsm_wrapper_sources *.cc)

add_library (_cbdsm MODULE ${cbdsm_wrapper_sources})

target_link_libraries (_cbdsm 
  minpack
  port3
  ${NUM_UTIL_LIBRARIES}
  ${BOOST_PYTHON_LIBRARY}
  )

set_target_properties (_cbdsm PROPERTIES PREFIX ""
  COMPILE_FLAGS "-Wall -Wextra -Wno-unused"
  LINKER_LANGUAGE C
  )

if (APPLE)
  set_target_properties(_cbdsm PROPERTIES 
    LINK_FLAGS "-undefined dynamic_lookup")
endif (APPLE)


## ----------------------------------------------------------------------
## Installation

install (TARGETS _cbdsm DESTINATION lib/python)
