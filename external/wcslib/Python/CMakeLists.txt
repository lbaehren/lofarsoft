##------------------------------------------------------------------------------
## $Id$
##------------------------------------------------------------------------------

set(WCSLIB_wrapper_sources wrapper.cc)

include_directories(
  ${WCSLIB_SOURCE_DIR}/C
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
)

if (BOOST_boost_python AND PYTHON_EXECUTABLE)
  add_library (wcslib MODULE ${WCSLIB_wrapper_sources})
  target_link_libraries (wcslib wcs ${BOOST_PYTHON_LIBRARY})
  set_target_properties (wcslib PROPERTIES PREFIX "")
else (BOOST_boost_python AND PYTHON_EXECUTABLE)
  message(ERROR "Python wrappers could not be built")
endif (BOOST_boost_python AND PYTHON_EXECUTABLE)

## Linking loadable modules on Mac is slightly more trickier
if(APPLE)
  set_target_properties(wcslib PROPERTIES 
      LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS wcslib
  LIBRARY DESTINATION lib/python
  )
