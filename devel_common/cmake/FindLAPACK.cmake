
## -----------------------------------------------------------------------------
## Check for the presence of LAPACK
##
## The following variables are set when LAPACK is found:
##  HAVE_LAPACK     - Set to true, if all components of LAPACK have been found.
##  LAPACK_LIBRARIES - Link these to use LAPACK

## -----------------------------------------------------------------------------
## Check for the library files (-llapack -lblas -lcblas -latlas)

set (LAPACK_SEARCH_PATH
  /usr/local/lib
  /usr/lib
  /lib 
  /sw/lib
  )

find_library (LAPACK_libatlas atlas ${LAPACK_SEARCH_PATH})
find_library (LAPACK_libblas blas ${LAPACK_SEARCH_PATH})
find_library (LAPACK_libcblas cblas ${LAPACK_SEARCH_PATH})
find_library (LAPACK_liblapack lapack ${LAPACK_SEARCH_PATH})

if (LAPACK_libatlas)
  list (APPEND LAPACK_LIBRARIES ${LAPACK_libatlas})
endif (LAPACK_libatlas)

if (LAPACK_libblas)
  list (APPEND LAPACK_LIBRARIES ${LAPACK_libblas})
endif (LAPACK_libblas)

if (LAPACK_libcblas)
  list (APPEND LAPACK_LIBRARIES ${LAPACK_libcblas})
endif (LAPACK_libcblas)

if (LAPACK_liblapack)
  list (APPEND LAPACK_LIBRARIES ${LAPACK_liblapack})
endif (LAPACK_liblapack)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (LAPACK_LIBRARIES)
  SET (HAVE_LAPACK TRUE)
ELSE (LAPACK_LIBRARIES)
  IF (NOT LAPACK_FIND_QUIETLY)
    IF (NOT LAPACK_LIBRARIES)
      MESSAGE (STATUS "Unable to find LAPACK library files!")
    ENDIF (NOT LAPACK_LIBRARIES)
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ENDIF (LAPACK_LIBRARIES)

IF (HAVE_LAPACK)
  IF (NOT LAPACK_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LAPACK")
    MESSAGE (STATUS "LAPACK_LIBRARIES     = ${LAPACK_LIBRARIES}")
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ELSE (HAVE_LAPACK)
  IF (LAPACK_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LAPACK!")
  ENDIF (LAPACK_FIND_REQUIRED)
ENDIF (HAVE_LAPACK)

MARK_AS_ADVANCED (
  HAVE_LAPACK
  LAPACK_LIBRARIES
  )
