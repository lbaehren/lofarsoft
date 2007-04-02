# - Check for the presence of Glish
#
# The following variables are set when Blitz++ is found:
#  HAVE_GLISH       = Set to true, if all components of Glish
#                          have been found.
#  Glish_INCLUDE_DIR = Include path for the header files of Glish
#  Glish_LIBRARY     = Link these to use Glish

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GLISH_INCLUDE_DIR glish.h
  PATHS
  /casa
  /opt/casa
  /sw/share/casa
  PATH_SUFFIXES
  code/aips/glish/include/Glish
  stable/code/aips/glish/include/Glish
  weekly/code/aips/glish/include/Glish
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (Glish_LIBRARY <package name>
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (Glish_INCLUDE_DIR AND Glish_LIBRARY)
  SET (HAVE_GLISH TRUE)
ELSE (Glish_INCLUDE_DIR AND Glish_LIBRARY)
  IF (NOT Glish_FIND_QUIETLY)
    IF (NOT Glish_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find Glish header files!")
    ENDIF (NOT Glish_INCLUDE_DIR)
    IF (NOT Glish_LIBRARY)
      MESSAGE (STATUS "Unable to find Glish library files!")
    ENDIF (NOT Glish_LIBRARY)
  ENDIF (NOT Glish_FIND_QUIETLY)
ENDIF (Glish_INCLUDE_DIR AND Glish_LIBRARY)

IF (HAVE_GLISH)
  IF (NOT Glish_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Glish")
    MESSAGE (STATUS "Glish_INCLUDE_DIR = ${Glish_INCLUDE_DIR}")
    MESSAGE (STATUS "Glish_LIBRARY     = ${Glish_LIBRARY}")
  ENDIF (NOT Glish_FIND_QUIETLY)
ELSE (HAVE_GLISH)
  IF (Glish_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Glish!")
  ENDIF (Glish_FIND_REQUIRED)
ENDIF (HAVE_GLISH)

## -----------------------------------------------------------------------------
## Mark as advanced ...
