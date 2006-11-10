# - Check for the presence of ROOT
#
# The following variables are set when Blitz++ is found:
#  ROOT_FOUND       = Set to true, if all components of ROOT
#                          have been found.
#  ROOT_INCLUDE_DIR = Include path for the header files of ROOT
#  ROOT_LIBRARY     = Link these to use ROOT

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (ROOT_INCLUDE_DIR TObject.h TROOT.h
	PATHS /usr/share /usr/local/share /sw/share
	PATH_SUFFIXES root/include
)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (ROOT_LIBRARY Cint MathCore
	PATHS /usr/share /usr/local/share /sw/share
	PATH_SUFFIXES root/lib
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (ROOT_INCLUDE_DIR AND ROOT_LIBRARY)
	SET (ROOT_FOUND TRUE)
ELSE (ROOT_INCLUDE_DIR AND ROOT_LIBRARY)
	IF (NOT ROOT_FIND_QUIETLY)
		IF (NOT ROOT_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find ROOT header files!")
		ENDIF (NOT ROOT_INCLUDE_DIR)
		IF (NOT ROOT_LIBRARY)
			MESSAGE (STATUS "Unable to find ROOT library files!")
		ENDIF (NOT ROOT_LIBRARY)
	ENDIF (NOT ROOT_FIND_QUIETLY)
ENDIF (ROOT_INCLUDE_DIR AND ROOT_LIBRARY)

IF (ROOT_FOUND)
	IF (NOT ROOT_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for ROOT")
		MESSAGE (STATUS "ROOT_INCLUDE_DIR = ${ROOT_INCLUDE_DIR}")
		MESSAGE (STATUS "ROOT_LIBRARY     = ${ROOT_LIBRARY}")
	ENDIF (NOT ROOT_FIND_QUIETLY)
ELSE (ROOT_FOUND)
	IF (ROOT_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find ROOT!")
	ENDIF (ROOT_FIND_REQUIRED)
ENDIF (ROOT_FOUND)

MARK_AS_ADVANCED (
	ROOT_LIBRARY
	ROOT_INCLUDE_DIR
)
