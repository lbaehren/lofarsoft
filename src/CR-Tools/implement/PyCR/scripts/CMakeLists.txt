if (CR_WITH_PYCR)

    # Install Python python_scripts
    file (GLOB python_scripts *.py)

    install (PROGRAMS ${python_scripts}
            DESTINATION bin/python
            )

endif (CR_WITH_PYCR)
