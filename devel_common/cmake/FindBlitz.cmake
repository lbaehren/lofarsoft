# - Check for the presence of the Blitz++ library
#
# The following variables are set when Blitz++ is found:
#  HAVE_BLITZ        = True when both Blitz++ header and include files are 
#                      found; if any of them is missing, HAVE_BLITZ=false
#  BLITZ_INCLUDE_DIR = the path to where the boost include files are.
#  BLITZ_LIBRARY     = Link these to use Blitz++

#SET (BLITZ_VERSION "1_33_1")

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (BLITZ_INCLUDE_DIR array.h
  PATHS /usr/local/include /usr/include /sw/include
  PATH_SUFFIXES blitz
  )

IF (BLITZ_INCLUDE_DIR)
  STRING (REGEX REPLACE include/blitz include BLITZ_INCLUDE_DIR ${BLITZ_INCLUDE_DIR})
ENDIF (BLITZ_INCLUDE_DIR)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (BLITZ_LIBRARY blitz
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BLITZ_INCLUDE_DIR AND BLITZ_LIBRARY)
  SET (HAVE_BLITZ TRUE)
ELSE (BLITZ_INCLUDE_DIR AND BLITZ_LIBRARY)
  IF (NOT BLITZ_FIND_QUIETLY)
    IF (NOT BLITZ_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find Blitz header files!")
    ENDIF (NOT BLITZ_INCLUDE_DIR)
    IF (NOT BLITZ_LIBRARY)
      MESSAGE (STATUS "Unable to find Blitz library files!")
    ENDIF (NOT BLITZ_LIBRARY)
  ENDIF (NOT BLITZ_FIND_QUIETLY)
ENDIF (BLITZ_INCLUDE_DIR AND BLITZ_LIBRARY)

IF (HAVE_BLITZ)
  IF (NOT BLITZ_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Blitz")
    MESSAGE (STATUS "BLITZ_INCLUDE_DIR = ${BLITZ_INCLUDE_DIR}")
    MESSAGE (STATUS "BLITZ_LIBRARY     = ${BLITZ_LIBRARY}")
  ENDIF (NOT BLITZ_FIND_QUIETLY)
ELSE (HAVE_BLITZ)
  IF (BLITZ_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Blitz!")
  ENDIF (BLITZ_FIND_REQUIRED)
ENDIF (HAVE_BLITZ)

## ------------------------------------------------------------------------------
## Mark as advanced ...

MARK_AS_ADVANCED (
  HAVE_BLITZ
  BLITZ_LIBRARY
  BLITZ_INCLUDE_DIR
  )
