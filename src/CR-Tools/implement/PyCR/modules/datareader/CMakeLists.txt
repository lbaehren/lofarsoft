## =========================================================================
##
##  Python bindings to libcr
##
## =========================================================================

##__________________________________________________________
## Check required external packages

if (CR_WITH_PYCR)

  ## Required external package: Python
  if (NOT HAVE_PYTHON)
    message (STATUS "[CR-Tools] Generation of datareader disabled; missing Python!")
    set (CR_WITH_PYCR OFF)
  endif (NOT HAVE_PYTHON)
  ## Required external package: Boost++
  if (NOT HAVE_BOOST)
    message (STATUS "[CR-Tools] Generation of datareader disabled; missing Boost!")
    set (CR_WITH_PYCR OFF)
  endif (NOT HAVE_BOOST)
  ## Required external package: NumPy
  if (HAVE_NUMPY)
    include_directories (${NUMPY_INCLUDES})
  else (HAVE_NUMPY)
    message (STATUS "[CR-Tools] Generation of datareader disabled; missing NumPy!")
    set (CR_WITH_PYCR OFF)
  endif (HAVE_NUMPY)
  ## Required external package: NumUtil
  if (HAVE_NUM_UTIL)
    include_directories (${NUM_UTIL_INCLUDES})
  else (HAVE_NUM_UTIL)
    message (STATUS "[CR-Tools] Generation of datareader disabled; missing NumUtil!")
    set (CR_WITH_PYCR OFF)
  endif (HAVE_NUM_UTIL)

endif (CR_WITH_PYCR)

##__________________________________________________________
## Build the library

if (CR_WITH_PYCR)

  file (GLOB datareader_sources *.cc)

  ## compiler instructions
  add_library (_datareader MODULE
    ${datareader_sources}
    )
  ## linker instructions
  target_link_libraries(_datareader
    cr
    ${DAL1_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${CASA_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${HAVE_LIBZ}
    ${NUM_UTIL_LIBRARIES}
    )
  ## additional target properties
  if (APPLE)
    set_target_properties (_datareader
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON -fpermissive"
      LINK_FLAGS "-fPIC -flat_namespace"
      )
  else (APPLE)
    set_target_properties (_datareader
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON"
      LINK_FLAGS "-fPIC -shared -Wl"
      )
  endif (APPLE)

##__________________________________________________________
## Installation

# Install _datareader C++ module
install (TARGETS _datareader
        LIBRARY DESTINATION lib/python/pycr/datareader
        )
  
# Install datareader Python module
install (PROGRAMS __init__.py
        DESTINATION lib/python/pycr/datareader
        )

endif (CR_WITH_PYCR)

## =============================================================================
##
##  Test scripts
##
## =============================================================================

if (CR_BUILD_TESTS)
  add_subdirectory (test)
endif (CR_BUILD_TESTS)
