## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

add_subdirectory (bdsm)
add_subdirectory (minpack)
add_subdirectory (port3)

include_directories (
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${PYTHON_INCLUDES}/../../lib/python2.5/site-packages/numpy/core/include)

set (cbdsm_wrapper_sources cbdsm_main.cc 
    stat.cc
    MGFunction1.cc MGFunction2.cc
    Fitter_lmder.cc Fitter_dn2g.cc Fitter_dnsg.cc
    )

add_library (_cbdsm MODULE ${cbdsm_wrapper_sources})

target_link_libraries (_cbdsm 
		      minpack port3
		      ${NUM_UTIL_LIBRARIES} ${BOOST_boost_python})

set_target_properties (_cbdsm PROPERTIES PREFIX ""
		      COMPILE_FLAGS "-Wall -Wextra -Wno-unused")

if (APPLE)
  set_target_properties(_cbdsm PROPERTIES 
      LINK_FLAGS "-bundle_loader ${PYTHON_EXECUTABLE}")
endif (APPLE)


## ----------------------------------------------------------------------
## Installation
install (TARGETS _cbdsm DESTINATION lib/python)
