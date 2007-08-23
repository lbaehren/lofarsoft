##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of WCS
#
# The following variables are set when WCS is found:
#  HAVE_WCSLIB       = Set to true, if all components of WCS have been found.
#  WCSLIB_INCLUDES   = Include path for the header files of WCS
#  WCSLIB_LIBRARIES  = Link these to use WCS

## -----------------------------------------------------------------------------
## Search locations

set (include_locations
  ../release/include
  ../../release/include
  /usr/include
  /usr/local/include
  /sw/include
)

set (lib_locations
  ../release/lib
  ../../release/lib
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (WCSLIB_INCLUDES wcs.h
  PATHS ${include_locations}
  PATH_SUFFIXES wcslib
  NO_DEFAULT_PATH
  )

## correct the include path

if (WCSLIB_INCLUDES)
  string (REGEX REPLACE wcslib "" WCSLIB_INCLUDES ${WCSLIB_INCLUDES})
endif (WCSLIB_INCLUDES)


## -----------------------------------------------------------------------------
## Check for the library

find_library (WCSLIB_LIBRARIES wcs
  PATHS ${lib_locations}
  PATH_SUFFIXES darwin/lib linux_gnu/lib stable/linux_gnu/lib
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)
  SET (HAVE_WCSLIB TRUE)
ELSE (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)
  IF (NOT WCSLIB_FIND_QUIETLY)
    IF (NOT WCSLIB_INCLUDES)
      MESSAGE (STATUS "Unable to find WCS header files!")
    ENDIF (NOT WCSLIB_INCLUDES)
    IF (NOT WCSLIB_LIBRARIES)
      MESSAGE (STATUS "Unable to find WCS library files!")
    ENDIF (NOT WCSLIB_LIBRARIES)
  ENDIF (NOT WCSLIB_FIND_QUIETLY)
ENDIF (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)

IF (HAVE_WCSLIB)
  IF (NOT WCSLIB_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for WCS")
    MESSAGE (STATUS "WCSLIB_INCLUDES  = ${WCSLIB_INCLUDES}")
    MESSAGE (STATUS "WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")
  ENDIF (NOT WCSLIB_FIND_QUIETLY)
ELSE (HAVE_WCSLIB)
  IF (WCSLIB_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find WCS!")
  ENDIF (WCSLIB_FIND_REQUIRED)
ENDIF (HAVE_WCSLIB)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  HAVE_WCSLIB
  WCSLIB_INCLUDES
  WCSLIB_LIBRARIES
)
