if (CR_WITH_PYCR)

    # Add packages
    set (packages
        datareader
        )

    foreach (package ${packages})
        add_subdirectory (${package})
    endforeach (package ${packages})

    # Install Python python_modules
    file (GLOB python_modules *.py)

    install (PROGRAMS ${python_modules}
            DESTINATION lib/python/pycr
            )

endif (CR_WITH_PYCR)
