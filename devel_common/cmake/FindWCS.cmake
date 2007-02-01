# - Check for the presence of WCS
#
# The following variables are set when WCS is found:
#  HAVE_WCS       = Set to true, if all components of WCS have been found.
#  WCS_INCLUDES   = Include path for the header files of WCS
#  WCS_LIBRARIES  = Link these to use WCS

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (WCS_INCLUDES wcs.h
  PATHS /usr/local/include /usr/include /sw/include /opt/casa /sw/share/casa
  PATH_SUFFIXES code/casa/wcslib stable/code/casa/wcslib
  )

## correct the include path

IF (WCS_INCLUDES)
  STRING (REGEX REPLACE wcslib "" WCS_INCLUDES ${WCS_INCLUDES})
ENDIF (WCS_INCLUDES)


## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (WCS_LIBRARIES wcs
  PATHS /usr/local/lib /usr/lib /lib /sw/lib /opt/casa /sw/share/casa
  PATH_SUFFIXES darwin/lib linux_gnu/lib stable/linux_gnu/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (WCS_INCLUDES AND WCS_LIBRARIES)
  SET (HAVE_WCS TRUE)
ELSE (WCS_INCLUDES AND WCS_LIBRARIES)
  IF (NOT WCS_FIND_QUIETLY)
    IF (NOT WCS_INCLUDES)
      MESSAGE (STATUS "Unable to find WCS header files!")
    ENDIF (NOT WCS_INCLUDES)
    IF (NOT WCS_LIBRARIES)
      MESSAGE (STATUS "Unable to find WCS library files!")
    ENDIF (NOT WCS_LIBRARIES)
  ENDIF (NOT WCS_FIND_QUIETLY)
ENDIF (WCS_INCLUDES AND WCS_LIBRARIES)

IF (HAVE_WCS)
  IF (NOT WCS_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for WCS")
    MESSAGE (STATUS "WCS_INCLUDES = ${WCS_INCLUDES}")
    MESSAGE (STATUS "WCS_LIBRARIES     = ${WCS_LIBRARIES}")
  ENDIF (NOT WCS_FIND_QUIETLY)
ELSE (HAVE_WCS)
  IF (WCS_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find WCS!")
  ENDIF (WCS_FIND_REQUIRED)
ENDIF (HAVE_WCS)

MARK_AS_ADVANCED (
  HAVE_WCS
  WCS_LIBRARIES
  WCS_INCLUDES
  )
