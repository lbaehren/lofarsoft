set (tests
    test_jd_date
    test_angles
    test_equatorial_horizontal
    test_equatorial_galactic
    )

foreach (test ${tests})
    add_executable (${test} ${test}.cc)

    target_link_libraries (${test} tmf)
endforeach (test ${tests})

