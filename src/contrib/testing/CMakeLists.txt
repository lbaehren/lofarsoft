
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
    )

  foreach (src ${tests})
    add_executable (${src} ${src}.cc)
    target_link_libraries (${src} ${CASA_LIBRARIES})
    add_test (${src} ${src})
  endforeach (src)
    
endif (HAVE_CASACORE)
