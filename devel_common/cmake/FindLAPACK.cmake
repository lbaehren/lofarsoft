
## -----------------------------------------------------------------------------
## Check for the presence of LAPACK
##
## The following variables are set when LAPACK is found:
##  HAVE_LAPACK     - Set to true, if all components of LAPACK have been found.
##  LAPACK_LIBRARY - Link these to use LAPACK

## -----------------------------------------------------------------------------
## Check for the library files (-llapack -lblas -lcblas -latlas)

FIND_LIBRARY (LAPACK_LIBRARY lapack blas cblas atlas
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (LAPACK_LIBRARY)
  SET (HAVE_LAPACK TRUE)
ELSE (LAPACK_LIBRARY)
  IF (NOT LAPACK_FIND_QUIETLY)
    IF (NOT LAPACK_LIBRARY)
      MESSAGE (STATUS "Unable to find LAPACK library files!")
    ENDIF (NOT LAPACK_LIBRARY)
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ENDIF (LAPACK_LIBRARY)

IF (HAVE_LAPACK)
  IF (NOT LAPACK_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LAPACK")
    MESSAGE (STATUS "LAPACK_LIBRARY     = ${LAPACK_LIBRARY}")
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ELSE (HAVE_LAPACK)
  IF (LAPACK_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LAPACK!")
  ENDIF (LAPACK_FIND_REQUIRED)
ENDIF (HAVE_LAPACK)

MARK_AS_ADVANCED (
  HAVE_LAPACK
  LAPACK_LIBRARY
  )
