
## ==============================================================================
##
##  Tests for casacore
##
## ==============================================================================

if (HAVE_CASACORE)

  include_directories (${CASA_INCLUDES})

  set (tests
    casacoreMatrixAccess01
    casacoreMatrixAccess02
    casacoreMatrixAccess03
    casacoreMatrixAccess04
    )

  foreach (src ${tests})
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src} ${CASA_LIBRARIES})
    ## testing
    add_test (${src} ${src})
  endforeach (src)
    
endif (HAVE_CASACORE)

## ==============================================================================
##
##  Tests for Boost
##
## ==============================================================================

if (HAVE_BOOST)

  set (tests
    boostProgramOptions
    )
  
  foreach (src ${tests})
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src} ${BOOST_LIBRARIES})
    ## testing
    add_test (${src} ${src})
  endforeach (src)
    
endif (HAVE_BOOST)