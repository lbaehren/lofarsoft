# - Check for the presence of the CFITSIO library
#
#  HAVE_CFITSIO        = Do we have CFITSIO?
#  CFITSIO_LIBRARY     = Location of the CFITSIO library (libcfitsio)
#  CFITSIO_LIBRARIES   = Set of libraries required for linking against CFITSIO
#  CFITSIO_INCLUDE_DIR = Directory where to find fitsio.h

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CFITSIO_INCLUDE_DIR
  fitsio.h longnam.h
  PATHS /usr/include /usr/local/include /sw/include /opt/casa/local/include
  PATH_SUFFIXES cfitsio
  )

## -----------------------------------------------------------------------------
## Check for the parts of the library

## [1] core library

FIND_LIBRARY (CFITSIO_LIBRARY
  NAMES cfitsio
  PATHS /lib /usr/lib /usr/local/lib /sw/lib /opt/casa/local/lib
  PATH_SUFFIXES cfitsio
)

if (CFITSIO_LIBRARY)
  set (CFITSIO_LIBRARIES ${CFITSIO_LIBRARY})
endif (CFITSIO_LIBRARY)

## [2] math library

FIND_LIBRARY (CFITSIO_libm
  NAMES m
  PATHS /lib /usr/lib /usr/local/lib /sw/lib /opt/casa/local/lib
)

if (CFITSIO_libm)
  list (APPEND CFITSIO_LIBRARIES ${CFITSIO_libm})
endif (CFITSIO_libm)

## [3] file access

FIND_LIBRARY (CFITSIO_libnsl
  NAMES nsl
  PATHS /lib /usr/lib /usr/local/lib /sw/lib /opt/casa/local/lib
  )

if (CFITSIO_libnsl)
  list (APPEND CFITSIO_LIBRARIES ${CFITSIO_libnsl})
endif (CFITSIO_libnsl)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)
  SET (HAVE_CFITSIO TRUE)
ELSE (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)
  IF (NOT CFITSIO_FIND_QUIETLY)
    IF (NOT CFITSIO_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find CFITSIO header files!")
    ENDIF (NOT CFITSIO_INCLUDE_DIR)
    IF (NOT CFITSIO_LIBRARIES)
      MESSAGE (STATUS "Unable to find CFITSIO library files!")
    ENDIF (NOT CFITSIO_LIBRARIES)
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ENDIF (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)


IF (HAVE_CFITSIO)
  IF (NOT CFITSIO_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for CFITSIO")
    MESSAGE (STATUS "CFITSIO_LIBRARIES   = ${CFITSIO_LIBRARIES}")
    MESSAGE (STATUS "CFITSIO_INCLUDE_DIR = ${CFITSIO_INCLUDE_DIR}")
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ELSE (HAVE_CFITSIO)
  IF (CFITSIO_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
  ENDIF (CFITSIO_FIND_REQUIRED)
ENDIF (HAVE_CFITSIO)

## ------------------------------------------------------------------------------
## Mark as advanced ...

MARK_AS_ADVANCED (
  HAVE_CFITSIO
  CFITSIO_LIBRARY
  CFITSIO_LIBRARIES
  CFITSIO_INCLUDE_DIR
  )
