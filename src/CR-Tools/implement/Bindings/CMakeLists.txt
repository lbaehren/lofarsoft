## =============================================================================
##
##  Python bindings to libcr
##
## =============================================================================

##__________________________________________________________
## Check required external packages

if (CR_WITH_PYCR)

  ## Required external package: Python
  if (NOT HAVE_PYTHON)
    message (STATUS "[CR-Tools] Generation of pycr disabled; missing Python!")
    set (CR_WITH_PYCR OFF)
  endif (NOT HAVE_PYTHON)
  ## Required external package: Boost++
  if (NOT HAVE_BOOST)
    message (STATUS "[CR-Tools] Generation of pycr disabled; missing Boost!")
    set (CR_WITH_PYCR OFF)
  endif (NOT HAVE_BOOST)
  ## Required external package: NumPy
  if (HAVE_NUMPY)
    include_directories (${NUMPY_INCLUDES})
  else (HAVE_NUMPY)
    message (STATUS "[CR-Tools] Generation of pycr disabled; missing NumPy!")
    set (CR_WITH_PYCR OFF)
  endif (HAVE_NUMPY)
  ## Required external package: NumUtil
  if (HAVE_NUM_UTIL)
    include_directories (${NUM_UTIL_INCLUDES})
  else (HAVE_NUM_UTIL)
    message (STATUS "[CR-Tools] Generation of pycr disabled; missing NumUtil!")
    set (CR_WITH_PYCR OFF)
  endif (HAVE_NUM_UTIL)

endif (CR_WITH_PYCR)

##__________________________________________________________
## Build the library

if (CR_WITH_PYCR)

  ## compiler instructions
  add_library (pycr MODULE
    ../Analysis/DynamicSpectrum.cc
    ../Coordinates/CoordinateType.cc
    ../Coordinates/TimeFreq.cc
    ../IO/DataIterator.cc
    ../IO/DataReader.cc
    ../IO/LOFAR_TBB.cc
    ../Math/BasicFilter.cc
    ../Math/HanningFilter.cc
    ../Coordinates/MConversions.cc
    pycr.cc
    )
  ## linker instructions
  target_link_libraries(pycr
    ${HAVE_LIBZ}
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${DAL_LIBRARIES}
    ${cr_link_libraries}
    )
  ## additional target properties
  if (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON -fpermissive"
      LINK_FLAGS "-fPIC -flat_namespace"
      )
  else (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON"
      LINK_FLAGS "-fPIC -shared -Wl"
      )
  endif (APPLE)
  ## installation
  install (
    TARGETS pycr
    LIBRARY DESTINATION lib/python
    )
  
endif (CR_WITH_PYCR)

