
## ==============================================================================
##
##  Test import of Python modules
##
## ==============================================================================

foreach (importTest
    test_import_presto
    test_import_sifting
    )
  
  add_test (${importTest} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${importTest}.py )

endforeach (importTest)
  
## ==============================================================================
##
##  Test program executables
##
## ==============================================================================

add_test (dspsr_startup dspsr)

