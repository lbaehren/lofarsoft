## ------------------------------------------------------------------------------
## Tests

set (TESTS
    tgetgrid
    tconvert
    )

foreach (TEST ${TESTS})
    add_executable (${TEST} ${TEST}.cc)
    
    target_link_libraries (${TEST} ${cr_link_libraries}) 
endforeach (TEST)

