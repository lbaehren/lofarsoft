##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of Glish
#
# The following variables are set when Blitz++ is found:
#  HAVE_GLISH       = Set to true, if all components of Glish have been found.
#  GLISH_INCLUDES = Include path for the header files of Glish
#  GLISH_LIBRARIES     = Link these to use Glish

set (casa_locations
  /casa
  /opt/casa
  /sw/share/casa
  ## CASA still might be installed under the old name
  /opt/aips++
  /app/aips++/Stable
  )

## -----------------------------------------------------------------------------
## Get some basic information on the system

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set (CASA_SYSTEM_NAME "darwin")
endif (CMAKE_SYSTEM_NAME MATCHES "Darwin")

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GLISH_INCLUDE_DIR glish.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  code/aips/glish/include/Glish
  stable/code/aips/glish/include/Glish
  weekly/code/aips/glish/include/Glish
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (GLISH_LIBRARIES <package name>
  PATHS ${lib_locations}
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (GLISH_INCLUDES AND GLISH_LIBRARIES)
  SET (HAVE_GLISH TRUE)
ELSE (GLISH_INCLUDES AND GLISH_LIBRARIES)
  IF (NOT Glish_FIND_QUIETLY)
    IF (NOT GLISH_INCLUDES)
      MESSAGE (STATUS "Unable to find Glish header files!")
    ENDIF (NOT GLISH_INCLUDES)
    IF (NOT GLISH_LIBRARIES)
      MESSAGE (STATUS "Unable to find Glish library files!")
    ENDIF (NOT GLISH_LIBRARIES)
  ENDIF (NOT Glish_FIND_QUIETLY)
ENDIF (GLISH_INCLUDES AND GLISH_LIBRARIES)

IF (HAVE_GLISH)
  IF (NOT Glish_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Glish")
    MESSAGE (STATUS "GLISH_INCLUDES  = ${GLISH_INCLUDES}")
    MESSAGE (STATUS "GLISH_LIBRARIES = ${GLISH_LIBRARIES}")
  ENDIF (NOT Glish_FIND_QUIETLY)
ELSE (HAVE_GLISH)
  IF (Glish_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Glish!")
  ENDIF (Glish_FIND_REQUIRED)
ENDIF (HAVE_GLISH)

## -----------------------------------------------------------------------------
## Mark as advanced ...
