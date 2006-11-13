# - Check for the presence of the CFITSIO library
#
#  CFITSIO_LIBRARY     = Location of the CFITSIO library
#  CFITSIO_INCLUDE_DIR = Directory where to find fitsio.h

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (CFITSIO_LIBRARY
  NAMES cfitsio
  PATHS /lib /usr/lib /usr/local/lib /sw/lib /opt/casa/local/lib
  PATH_SUFFIXES cfitsio
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CFITSIO_INCLUDE_DIR
	fitsio.h longnam.h
	PATHS /usr/include /usr/local/include /sw/include /opt/casa/local/include
	PATH_SUFFIXES cfitsio
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARY)
	SET (CFITSIO_FOUND TRUE)
ELSE (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARY)
	IF (NOT CFITSIO_FIND_QUIETLY)
		IF (NOT CFITSIO_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find CFITSIO header files!")
		ENDIF (NOT CFITSIO_INCLUDE_DIR)
		IF (NOT CFITSIO_LIBRARY)
			MESSAGE (STATUS "Unable to find CFITSIO library files!")
		ENDIF (NOT CFITSIO_LIBRARY)
	ENDIF (NOT CFITSIO_FIND_QUIETLY)
ENDIF (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARY)


IF (CFITSIO_FOUND)
	IF (NOT CFITSIO_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for CFITSIO")
		MESSAGE (STATUS "CFITSIO_LIBRARY     = ${CFITSIO_LIBRARY}")
		MESSAGE (STATUS "CFITSIO_INCLUDE_DIR = ${CFITSIO_INCLUDE_DIR}")
	ENDIF (NOT CFITSIO_FIND_QUIETLY)
ELSE (CFITSIO_FOUND)
	IF (CFITSIO_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
	ENDIF (CFITSIO_FIND_REQUIRED)
ENDIF (CFITSIO_FOUND)
