## =========================================================================
##
##  Python bindings to the Heino Falcke Library
##
## =========================================================================

##__________________________________________________________
## Check required external packages

include (FindPython)
include (FindBoost)
include (FindNumPy)
include (FindNumUtil)
include (FindTMF)

## Required external package: Python
if (NOT HAVE_PYTHON)
  message (STATUS "[HFL-Tools] Generation of pyhfl disabled; missing Python!")
  set (HFL_WITH_BINDINGS OFF)
endif (NOT HAVE_PYTHON)
## Required external package: Boost++
if (NOT HAVE_BOOST)
  message (STATUS "[HFL-Tools] Generation of pyhfl disabled; missing Boost!")
  set (HFL_WITH_BINDINGS OFF)
endif (NOT HAVE_BOOST)
## Required external package: NumPy
if (HAVE_NUMPY)
  include_directories (${NUMPY_INCLUDES})
else (HAVE_NUMPY)
  message (STATUS "[HFL-Tools] Generation of pyhfl disabled; missing NumPy!")
  set (HFL_WITH_BINDINGS OFF)
endif (HAVE_NUMPY)
## Required external package: NumUtil
if (HAVE_NUM_UTIL)
  include_directories (${NUM_UTIL_INCLUDES})
else (HAVE_NUM_UTIL)
  message (STATUS "[HFL-Tools] Generation of pyhfl disabled; missing NumUtil!")
  set (HFL_WITH_BINDINGS OFF)
endif (HAVE_NUM_UTIL)

##__________________________________________________________
## Build the library

if (HFL_WITH_BINDINGS)
  set (MODULES
      math
      beamforming
      coordinates
      fft
      )

  include_directories (${CR_SOURCE_DIR}/implement/PyCR)
  
  foreach (MODULE ${MODULES})
      file (GLOB module_sources ${MODULE}*.cc)
      
      ## compiler instructions
      add_library (${MODULE} MODULE
        ${module_sources}
        )
      
      ## linker instructions
      target_link_libraries(${MODULE}
        cr
        ${TMF_LIBRARIES}
        ${PYTHON_LIBRARIES}
        ${BOOST_LIBRARIES}
        ${NUM_UTIL_LIBRARIES}
        )
      
      ## additional target properties
      if (APPLE)
        set_target_properties (${MODULE}
          PROPERTIES
          PREFIX ""
          SUFFIX .so
          COMPILE_FLAGS "-DPYTHON -fpermissive"
          LINK_FLAGS "-fPIC -flat_namespace"
          )
      else (APPLE)
        set_target_properties (${MODULE}
          PROPERTIES
          PREFIX ""
          SUFFIX .so
          COMPILE_FLAGS "-DPYTHON"
          LINK_FLAGS "-fPIC -shared -Wl"
          )
      endif (APPLE)
  endforeach (MODULE)
  
  ##__________________________________________________________
  ## Installation

  if (CR_WITH_PYCR)
      set (DESTDIR lib/python/pycr/hfl)
  else (CR_WITH_PYCR)
      set (DESTDIR lib/python/hfl)
  endif (CR_WITH_PYCR)
  
  # Install package init script
  install (PROGRAMS __init__.py
          DESTINATION ${DESTDIR}
          )
  
  # Install modules
  foreach (MODULE ${MODULES})
      install (
        TARGETS ${MODULE}
        LIBRARY DESTINATION ${DESTDIR}
        )
  endforeach (MODULE)
  
  ## =======================================================================
  ##
  ##  Test scripts
  ##
  ## =======================================================================
  
  if (HFL_BUILD_TESTS)
    add_subdirectory (test)
  endif (HFL_BUILD_TESTS)

endif (HFL_WITH_BINDINGS)

