##------------------------------------------------------------------------------
## $Id$
##------------------------------------------------------------------------------

set(WCSLIB_wrapper_sources wrapper.cc)

include_directories(
  ${WCSLIB_SOURCE_DIR}/C
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
)

if (BOOST_PYTHON_LIBRARY AND PYTHON_EXECUTABLE)
  ## compiler instructions
  add_library (wcslib MODULE ${WCSLIB_wrapper_sources})
  ## linker instructions
  target_link_libraries (wcslib wcs ${BOOST_PYTHON_LIBRARY})
  ## target properties
  set_target_properties (wcslib PROPERTIES PREFIX "")
  ## additional target properties for Mac OS X
  if(APPLE)
    set_target_properties(wcslib PROPERTIES 
      LINK_FLAGS "-undefined dynamic_lookup")
  endif(APPLE)
  ## Installation
  install (
    TARGETS wcslib
    LIBRARY DESTINATION lib/python
    )
else (BOOST_boost_python AND PYTHON_EXECUTABLE)
  message(ERROR "Python wrappers could not be built")
endif (BOOST_PYTHON_LIBRARY AND PYTHON_EXECUTABLE)
