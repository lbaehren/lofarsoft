# - Check for the presence of the PGPLOT library
#
# Defines the following variables:
#  PGPLOT_LIBRARIES    -- Path to all parts of the PGPLOT library
#  PGPLOT_LIBRARY      -- Path to pgplot.{a,so}
#  PGPLOT_INCLUDE_DIR  -- Directory containing the PGPLOT header files
#  CPGPLOT_LIBRARY     -- Path to cpgplot.{a,so}
#  CPGPLOT_INCLUDE_DIR -- Directory containing the CPGPLOT header files
#  XmPGPLOT_LIBRARY    -- Path to libXmPgplot.{a,so}

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (PGPLOT_LIBRARY 
  NAMES pgplot
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES pgplot
  )

FIND_LIBRARY (CPGPLOT_LIBRARY
  NAMES cpgplot
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES pgplot
  )

FIND_LIBRARY (XmPGPLOT_LIBRARY
  NAMES XmPgplot
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
  SET (PGPLOT_FOUND TRUE)
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

IF (PGPLOT_LIBRARY)
  SET (PGPLOT_LIBRARIES "${PGPLOT_LIBRARY}")
  IF (CPGPLOT_LIBRARY)
    SET (PGPLOT_LIBRARIES "${CPGPLOT_LIBRARY} ${PGPLOT_LIBRARIES}")
  ENDIF (CPGPLOT_LIBRARY)
  IF (XmPGPLOT_LIBRARY)
    SET (PGPLOT_LIBRARIES "${XmPGPLOT_LIBRARY} ${PGPLOT_LIBRARIES}")
  ENDIF (XmPGPLOT_LIBRARY)
ELSE (PGPLOT_LIBRARY)
  SET (PGPLOT_LIBRARIES "")
ENDIF (PGPLOT_LIBRARY)

IF (PGPLOT_FOUND)
  IF (NOT PGPLOT_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for PGPLOT")
    MESSAGE (STATUS "PGPLOT_LIBRARIES    = ${PGPLOT_LIBRARIES}")
    MESSAGE (STATUS "PGPLOT_LIBRARY      = ${PGPLOT_LIBRARY}")
    MESSAGE (STATUS "PGPLOT_INCLUDE_DIR  = ${PGPLOT_INCLUDE_DIR}")
    MESSAGE (STATUS "CPGPLOT_LIBRARY     = ${CPGPLOT_LIBRARY}")
    MESSAGE (STATUS "CPGPLOT_INCLUDE_DIR = ${CPGPLOT_INCLUDE_DIR}")
    MESSAGE (STATUS "XmPGPLOT_LIBRARY    = ${XmPGPLOT_LIBRARY}")
  ENDIF (NOT PGPLOT_FIND_QUIETLY)
ELSE (PGPLOT_FOUND)
  IF (PGPLOT_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find PGPLOT!")
  ENDIF (PGPLOT_FIND_REQUIRED)
ENDIF (PGPLOT_FOUND)

## -----------------------------------------------------------------------------

MARK_AS_ADVANCED(
  PGPLOT_LIBRARY
  CPGPLOT_LIBRARY
  XmPGPLOT_LIBRARY
  )
