## ------------------------------------------------------------------------------
## Tests

set (TESTS
    tgetgrid
    tconvert
    )

include_directories (${CR_SOURCE_DIR}/implement/PyCR)

foreach (TEST ${TESTS})
    add_executable (${TEST} ${TEST}.cc)
    
    target_link_libraries (${TEST} ${cr_link_libraries}) 
endforeach (TEST)

