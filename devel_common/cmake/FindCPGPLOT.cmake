# - Check for the presence of the CPGPLOT library
#

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (CPGPLOT_LIBRARIES
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

IF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARIES)
	SET (CPGPLOT_FOUND TRUE)
ELSE (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARIES)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		IF (NOT CPGPLOT_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find CPGPLOT header files!")
		ENDIF (NOT CPGPLOT_INCLUDE_DIR)
		IF (NOT CPGPLOT_LIBRARIES)
			MESSAGE (STATUS "Unable to find CPGPLOT library files!")
		ENDIF (NOT CPGPLOT_LIBRARIES)
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ENDIF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARIES)


IF (CPGPLOT_FOUND)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for CPGPLOT")
		MESSAGE (STATUS "CPGPLOT library : ${CPGPLOT_LIBRARIES}")
		MESSAGE (STATUS "CPGPLOT headers : ${CPGPLOT_INCLUDE_DIR}")
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ELSE (CPGPLOT_FOUND)
	IF (CPGPLOT_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find CPGPLOT!")
	ENDIF (CPGPLOT_FIND_REQUIRED)
ENDIF (CPGPLOT_FOUND)
