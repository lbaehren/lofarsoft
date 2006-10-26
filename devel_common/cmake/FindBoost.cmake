# - Check for the presence of the Boost library
#

SET (Boost_VERSION 1_33_1)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (Boost_LIBRARIES
  NAMES boost_python-${Boost_VERSION} boost_wave-${Boost_VERSION}
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (Boost_INCLUDE_DIR
	boost.h
	PATHS /usr/local/include /usr/include /sw/include
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (Boost_INCLUDE_DIR AND Boost_LIBRARIES)
	SET (Boost_FOUND TRUE)
ELSE (Boost_INCLUDE_DIR AND Boost_LIBRARIES)
	IF (NOT Boost_FIND_QUIETLY)
		IF (NOT Boost_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find Boost header files!")
		ENDIF (NOT Boost_INCLUDE_DIR)
		IF (NOT Boost_LIBRARIES)
			MESSAGE (STATUS "Unable to find Boost library files!")
		ENDIF (NOT Boost_LIBRARIES)
	ENDIF (NOT Boost_FIND_QUIETLY)
ENDIF (Boost_INCLUDE_DIR AND Boost_LIBRARIES)

IF (Boost_FOUND)
	IF (NOT Boost_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for Boost")
		MESSAGE (STATUS "Boost library : ${Boost_LIBRARIES}")
		MESSAGE (STATUS "Boost headers : ${Boost_INCLUDE_DIR}")
	ENDIF (NOT Boost_FIND_QUIETLY)
ELSE (Boost_FOUND)
	IF (Boost_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find Boost!")
	ENDIF (Boost_FIND_REQUIRED)
ENDIF (Boost_FOUND)
