if (CR_WITH_PYCR)

    # Install Python python_modules
    file (GLOB python_modules *.py)

    install (PROGRAMS ${python_modules}
            DESTINATION lib/python/pycr
            )

endif (CR_WITH_PYCR)
