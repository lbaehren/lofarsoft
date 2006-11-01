# - Check for the presence of the Boost library
#

SET (Boost_VERSION 1_32)

## -----------------------------------------------------------------------------
## Check for the library

SET (Boost_LIBRARIES "")

FIND_LIBRARY (Boost_python_LIBRARY
	NAMES boost_python-${Boost_VERSION}
	PATHS /usr/local/lib /usr/lib /lib /sw/lib
	PATH_SUFFIXES boost
)

IF (Boost_python_LIBRARY)
	SET (Boost_LIBRARIES "${Boost_LIBRARIES} ${Boost_python_LIBRARY}")
ENDIF (Boost_python_LIBRARY)

FIND_LIBRARY (Boost_wave_LIBRARY
	NAMES boost_wave-${Boost_VERSION}
	PATHS /usr/local/lib /usr/lib /lib /sw/lib
	PATH_SUFFIXES boost
)

IF (Boost_wave_LIBRARY)
	SET (Boost_LIBRARIES "${Boost_LIBRARIES} ${Boost_wave_LIBRARY}")
ENDIF (Boost_wave_LIBRARY)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (Boost_INCLUDE_DIR
	config.hpp
	PATHS /usr/local/include /usr/include /sw/include
	PATH_SUFFIXES boost ${Boost_VERSION} ${Boost_VERSION}/boost
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
