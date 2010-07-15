## =========================================================================
## PyCR
## =========================================================================

if (CR_WITH_PYCR)
    add_subdirectory (core)
    add_subdirectory (ttl)
    add_subdirectory (modules)
    add_subdirectory (scripts)
endif (CR_WITH_PYCR)

