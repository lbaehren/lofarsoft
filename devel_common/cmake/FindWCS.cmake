# - Check for the presence of WCS
#
# The following variables are set when WCS is found:
#  HAVE_WCS        = Set to true, if all components of WCS have been found.
#  WCS_INCLUDE_DIR = Include path for the header files of WCS
#  WCS_LIBRARY     = Link these to use WCS

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (WCS_INCLUDE_DIR wcs.h
  PATHS /usr/local/include /usr/include /sw/include /opt/casa /sw/share/casa
  PATH_SUFFIXES code/casa/wcslib stable/code/casa/wcslib
  )

## correct the include path

IF (WCS_INCLUDE_DIR)
  STRING (REGEX REPLACE wcslib "" WCS_INCLUDE_DIR ${WCS_INCLUDE_DIR})
ENDIF (WCS_INCLUDE_DIR)


## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (WCS_LIBRARY wcs
  PATHS /usr/local/lib /usr/lib /lib /sw/lib /opt/casa /sw/share/casa
  PATH_SUFFIXES darwin/lib linux_gnu/lib stable/linux_gnu/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (WCS_INCLUDE_DIR AND WCS_LIBRARY)
  SET (HAVE_WCS TRUE)
ELSE (WCS_INCLUDE_DIR AND WCS_LIBRARY)
  IF (NOT WCS_FIND_QUIETLY)
    IF (NOT WCS_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find WCS header files!")
    ENDIF (NOT WCS_INCLUDE_DIR)
    IF (NOT WCS_LIBRARY)
      MESSAGE (STATUS "Unable to find WCS library files!")
    ENDIF (NOT WCS_LIBRARY)
  ENDIF (NOT WCS_FIND_QUIETLY)
ENDIF (WCS_INCLUDE_DIR AND WCS_LIBRARY)

IF (HAVE_WCS)
  IF (NOT WCS_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for WCS")
    MESSAGE (STATUS "WCS_INCLUDE_DIR = ${WCS_INCLUDE_DIR}")
    MESSAGE (STATUS "WCS_LIBRARY     = ${WCS_LIBRARY}")
  ENDIF (NOT WCS_FIND_QUIETLY)
ELSE (HAVE_WCS)
  IF (WCS_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find WCS!")
  ENDIF (WCS_FIND_REQUIRED)
ENDIF (HAVE_WCS)

MARK_AS_ADVANCED (
  HAVE_WCS
  WCS_LIBRARY
  WCS_INCLUDE_DIR
  )
