
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

## Test import of individual functions/symbols from module 'sifting'

foreach (importSifting
    DM_re
    Num
#    __builtins__
#    __doc__
#    __file__
#    __name__
#    __package__
    c_pow_threshold
    candidate
    candidate_sigma
    cmp_dms
    cmp_freq
    cmp_sigma
    cmp_snr
    file_candidates
    fund_re
    harm_pow_cutoff
    harms_re
    known_birds_f
    known_birds_p
    long_period
    parse_power
    print_sift_globals
    r_err
    re
    read_candidates
    remove_DM_problems
    remove_duplicate_candidates
    remove_harmonics
    short_period
    sifting
    sigma_threshold
    sys
    write_candlist
    )
  
  ## Generate test script
  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/test_import_sifting.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_import_sifting_${importSifting}.py
    )

  ## Set up test
    add_test (test_import_sifting_${importSifting}
      ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/test_import_sifting_${importSifting}.py
      )
  
endforeach (importSifting)

  
## ==============================================================================
##
##  Test program executables
##
## ==============================================================================

add_test (dspsr_startup dspsr)

