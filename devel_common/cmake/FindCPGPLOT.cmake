# - Check for the presence of the CPGPLOT library
#
# Defines the following variables:
#  CPGPLOT_LIBRARY     -- Path to cpgplot.{a,so}
#  CPGPLOT_INCLUDE_DIR -- Directory containing the CPGPLOT header files

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (CPGPLOT_LIBRARY
  NAMES cpgplot
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES pgplot
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CPGPLOT_INCLUDE_DIR
	cpgplot.h
	PATHS /usr/local /usr /sw
	PATH_SUFFIXES include/pgplot lib/pgplot
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)
	SET (CPGPLOT_FOUND TRUE)
ELSE (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		IF (NOT CPGPLOT_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find CPGPLOT header files!")
		ENDIF (NOT CPGPLOT_INCLUDE_DIR)
		IF (NOT CPGPLOT_LIBRARY)
			MESSAGE (STATUS "Unable to find CPGPLOT library files!")
		ENDIF (NOT CPGPLOT_LIBRARY)
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ENDIF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)


IF (CPGPLOT_FOUND)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for CPGPLOT")
		MESSAGE (STATUS "CPGPLOT_LIBRARY     = ${CPGPLOT_LIBRARY}")
		MESSAGE (STATUS "CPGPLOT_INCLUDE_DIR = ${CPGPLOT_INCLUDE_DIR}")
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ELSE (CPGPLOT_FOUND)
	IF (CPGPLOT_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find CPGPLOT!")
	ENDIF (CPGPLOT_FIND_REQUIRED)
ENDIF (CPGPLOT_FOUND)
