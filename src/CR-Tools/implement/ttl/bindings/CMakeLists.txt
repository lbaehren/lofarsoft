## =============================================================================
##
##  Python bindings to the Transient Template Library
##
## =============================================================================

##__________________________________________________________
## Check required external packages

include (FindPython)
include (FindBoost)
include (FindNumPy)
include (FindNumUtil)

## Required external package: Python
if (NOT HAVE_PYTHON)
  message (STATUS "[TTL-Tools] Generation of pyttl disabled; missing Python!")
  set (TTL_WITH_BINDINGS OFF)
endif (NOT HAVE_PYTHON)
## Required external package: Boost++
if (NOT HAVE_BOOST)
  message (STATUS "[TTL-Tools] Generation of pyttl disabled; missing Boost!")
  set (TTL_WITH_BINDINGS OFF)
endif (NOT HAVE_BOOST)
## Required external package: NumPy
if (HAVE_NUMPY)
  include_directories (${NUMPY_INCLUDES})
else (HAVE_NUMPY)
  message (STATUS "[TTL-Tools] Generation of pyttl disabled; missing NumPy!")
  set (TTL_WITH_BINDINGS OFF)
endif (HAVE_NUMPY)
## Required external package: NumUtil
if (HAVE_NUM_UTIL)
  include_directories (${NUM_UTIL_INCLUDES})
else (HAVE_NUM_UTIL)
  message (STATUS "[TTL-Tools] Generation of pyttl disabled; missing NumUtil!")
  set (TTL_WITH_BINDINGS OFF)
endif (HAVE_NUM_UTIL)

if (TTL_WITH_BINDINGS)

  ##__________________________________________________________
  ## Build the library


  set (MODULES
    math
    beamforming
    coordinates
    fft
    )

  foreach (MODULE ${MODULES})
    file (GLOB module_sources ${MODULE}*.cc)

    ## compiler instructions
    add_library (${MODULE} MODULE
      ${module_sources}
      )

    ## linker instructions
    target_link_libraries(${MODULE}
      cr
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

  # Install package init script
  install (PROGRAMS __init__.py
    DESTINATION lib/python/ttl
    )

  # Install modules
  foreach (MODULE ${MODULES})
    install (
      TARGETS ${MODULE}
      LIBRARY DESTINATION lib/python/ttl
      )
  endforeach (MODULE)

  ## =============================================================================
  ##
  ##  Test scripts
  ##
  ## =============================================================================

  if (TTL_BUILD_TESTS)
    add_subdirectory (test)
  endif (TTL_BUILD_TESTS)

endif (TTL_WITH_BINDINGS)
