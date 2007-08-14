##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of WCS
#
# The following variables are set when WCS is found:
#  HAVE_WCS       = Set to true, if all components of WCS have been found.
#  WCS_INCLUDES   = Include path for the header files of WCS
#  WCS_LIBRARIES  = Link these to use WCS

## -----------------------------------------------------------------------------
## Search locations

set (include_locations
  /usr/include
  /usr/local/include
  /sw/include
  /opt/casa/local/include
  ../release/include
  ../../release/include
)

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt/casa/local/lib
  ../release/lib
  ../../release/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (WCS_INCLUDES wcs.h
  PATHS ${include_locations}
  PATH_SUFFIXES wcslib
  )

## correct the include path

if (WCS_INCLUDES)
  string (REGEX REPLACE wcslib "" WCS_INCLUDES ${WCS_INCLUDES})
endif (WCS_INCLUDES)


## -----------------------------------------------------------------------------
## Check for the library

find_library (WCS_LIBRARIES wcs
  PATHS ${lib_locations}
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
    MESSAGE (STATUS "WCS_INCLUDES  = ${WCS_INCLUDES}")
    MESSAGE (STATUS "WCS_LIBRARIES = ${WCS_LIBRARIES}")
  ENDIF (NOT WCS_FIND_QUIETLY)
ELSE (HAVE_WCS)
  IF (WCS_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find WCS!")
  ENDIF (WCS_FIND_REQUIRED)
ENDIF (HAVE_WCS)

## ------------------------------------------------------------------------------
## Mark as advanced ...
