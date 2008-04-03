##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of the CFITSIO library
#
#  HAVE_CFITSIO      = Do we have CFITSIO?
#  CFITSIO_LIBRARIES = Set of libraries required for linking against CFITSIO
#  CFITSIO_INCLUDES  = Directory where to find fitsio.h

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CFITSIO_INCLUDES
  fitsio.h longnam.h
  PATHS ${include_locations}
  PATH_SUFFIXES cfitsio
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the parts of the library

## [1] core library

FIND_LIBRARY (CFITSIO_libcfitsio
  NAMES cfitsio
  PATHS ${lib_locations}
  PATH_SUFFIXES cfitsio
  NO_DEFAULT_PATH
)

if (CFITSIO_libcfitsio)
  set (CFITSIO_LIBRARIES ${CFITSIO_libcfitsio})
endif (CFITSIO_libcfitsio)

## [2] math library

FIND_LIBRARY (CFITSIO_libm
  NAMES m
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
)

if (CFITSIO_libm)
  list (APPEND CFITSIO_LIBRARIES ${CFITSIO_libm})
else (CFITSIO_libm)
  message (STATUS "Unable to find libm.")
endif (CFITSIO_libm)

## [3] file access

FIND_LIBRARY (CFITSIO_libnsl
  NAMES nsl
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (CFITSIO_libnsl)
  list (APPEND CFITSIO_LIBRARIES ${CFITSIO_libnsl})
else (CFITSIO_libnsl)
  message (STATUS "Unable to find libnsl.")
endif (CFITSIO_libnsl)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
  SET (HAVE_CFITSIO TRUE)
ELSE (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
  IF (NOT CFITSIO_FIND_QUIETLY)
    IF (NOT CFITSIO_INCLUDES)
      MESSAGE (STATUS "Unable to find CFITSIO header files!")
    ENDIF (NOT CFITSIO_INCLUDES)
    IF (NOT CFITSIO_LIBRARIES)
      MESSAGE (STATUS "Unable to find CFITSIO library files!")
    ENDIF (NOT CFITSIO_LIBRARIES)
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ENDIF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)

IF (HAVE_CFITSIO)
  IF (NOT CFITSIO_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for CFITSIO")
    MESSAGE (STATUS "CFITSIO_LIBRARIES = ${CFITSIO_LIBRARIES}")
    MESSAGE (STATUS "CFITSIO_INCLUDES  = ${CFITSIO_INCLUDES}")
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ELSE (HAVE_CFITSIO)
  IF (CFITSIO_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
  ENDIF (CFITSIO_FIND_REQUIRED)
ENDIF (HAVE_CFITSIO)

## ------------------------------------------------------------------------------
## Mark as advanced ...

MARK_AS_ADVANCED (
  CFITSIO_libcfitsio
  CFITSIO_libm
  CFITSIO_INCLUDES
  CFITSIO_LIBRARIES
  )

if (CFITSIO_libnsl)
  mark_as_advanced (
    CFITSIO_libnsl
    )
endif (CFITSIO_libnsl)
