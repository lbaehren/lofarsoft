# - Check for the presence of the Blitz++ library
#
# The following variables are set when Blitz++ is found:
#  Blitz_FOUND       = True when both Blitz++ header and include files are 
#                      found; if any of them is missing, Blitz_FOUND=false
#  Blitz_INCLUDE_DIR = the path to where the boost include files are.
#  Blitz_LIBRARY     = Link these to use Blitz++

#SET (Blitz_VERSION "1_33_1")

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (Blitz_INCLUDE_DIR array.h
  PATHS /usr/local/include /usr/include /sw/include
  PATH_SUFFIXES blitz
  )

IF (Blitz_INCLUDE_DIR)
  STRING (REGEX REPLACE include/blitz include Blitz_INCLUDE_DIR ${Blitz_INCLUDE_DIR})
ENDIF (Blitz_INCLUDE_DIR)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (Blitz_LIBRARY blitz
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (Blitz_INCLUDE_DIR AND Blitz_LIBRARY)
  SET (Blitz_FOUND TRUE)
ELSE (Blitz_INCLUDE_DIR AND Blitz_LIBRARY)
  IF (NOT Blitz_FIND_QUIETLY)
    IF (NOT Blitz_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find Blitz header files!")
    ENDIF (NOT Blitz_INCLUDE_DIR)
    IF (NOT Blitz_LIBRARY)
      MESSAGE (STATUS "Unable to find Blitz library files!")
    ENDIF (NOT Blitz_LIBRARY)
  ENDIF (NOT Blitz_FIND_QUIETLY)
ENDIF (Blitz_INCLUDE_DIR AND Blitz_LIBRARY)

IF (Blitz_FOUND)
  IF (NOT Blitz_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Blitz")
    MESSAGE (STATUS "Blitz_INCLUDE_DIR = ${Blitz_INCLUDE_DIR}")
    MESSAGE (STATUS "Blitz_LIBRARY     = ${Blitz_LIBRARY}")
  ENDIF (NOT Blitz_FIND_QUIETLY)
ELSE (Blitz_FOUND)
  IF (Blitz_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Blitz!")
  ENDIF (Blitz_FIND_REQUIRED)
ENDIF (Blitz_FOUND)

MARK_AS_ADVANCED (
  Blitz_LIBRARY
  Blitz_INCLUDE_DIR
  )
