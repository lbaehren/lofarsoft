# - Check for the presence of the CFITSIO library
#

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (CFITSIO_LIBRARIES
  NAMES cfitsio
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES cfitsio
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CFITSIO_INCLUDE_DIR
	fitsio.h
	PATHS /usr/local/include /usr/include /sw/include
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)
	SET (CFITSIO_FOUND TRUE)
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


IF (CFITSIO_FOUND)
	IF (NOT CFITSIO_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for CFITSIO")
		MESSAGE (STATUS "CFITSIO library : ${CFITSIO_LIBRARIES}")
		MESSAGE (STATUS "CFITSIO headers : ${CFITSIO_INCLUDE_DIR}")
	ENDIF (NOT CFITSIO_FIND_QUIETLY)
ELSE (CFITSIO_FOUND)
	IF (CFITSIO_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
	ENDIF (CFITSIO_FIND_REQUIRED)
ENDIF (CFITSIO_FOUND)
