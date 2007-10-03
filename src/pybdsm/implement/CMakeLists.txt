## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

add_subdirectory (bdsm)

include_directories (
  ${BOOST_INCLUDES}/..
  ${PYTHON_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${PYTHON_INCLUDES}/../../lib/python2.5/site-packages/numpy/core/include)


set (cbdsm_wrapper_sources cbdsm_main.cc stat.cc)

add_library (_cbdsm MODULE ${cbdsm_wrapper_sources})
target_link_libraries (_cbdsm ${NUM_UTIL_LIBRARIES} ${BOOST_boost_python})
set_target_properties (_cbdsm PROPERTIES PREFIX "")

if (APPLE)
  set_target_properties(_cbdsm PROPERTIES 
      LINK_FLAGS "-bundle_loader ${PYTHON_EXECUTABLE}")
endif (APPLE)


## ----------------------------------------------------------------------
## Installation
install (TARGETS _cbdsm DESTINATION lib/python)
