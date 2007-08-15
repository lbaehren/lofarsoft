##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of Glish
#
# The following variables are set when Blitz++ is found:
#  HAVE_GLISH      = Set to true, if all components of Glish have been found.
#  GLISH_INCLUDES  = Include path for the header files of Glish
#  GLISH_LIBRARIES = Link these to use Glish

## -----------------------------------------------------------------------------
## Search locations

set (include_locations
  ../release/include
  ../../release/include
  code/aips/glish/glish/include/Glish
  code/aips/glish/include/Glish
  code/casa/code/aips/glish/glish/include/Glish
)

set (lib_locations
  ../release/lib
  ../../release/lib
  linux/lib
  linux_gnu/lib
  darwin/lib
)

set (casa_locations
  /aips++
  /casa
  /opt/aips++
  /opt/casa
  /sw/share/aips++
  /sw/share/casa
  /opt/aips++
  /opt/casa
  /app/aips++/Stable
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GLISH_INCLUDES glish.h
  PATHS ${casa_locations}
  PATH_SUFFIXES ${include_locations}
  )

## -----------------------------------------------------------------------------
## Check for the library

## [1] libglish

find_library (GLISH_LIBRARIES glish
  PATHS ${casa_locations}
  PATH_SUFFIXES ${lib_locations}
  )

## [2] libsos

find_library (libsos sos
  PATHS ${casa_locations}
  PATH_SUFFIXES ${lib_locations}
  )

if (libsos)
  list (APPEND GLISH_LIBRARIES ${libsos})
endif (libsos)

## [3] libnpd

find_library (libnpd npd
  PATHS ${casa_locations}
  PATH_SUFFIXES ${lib_locations}
  )

if (libnpd)
  list (APPEND GLISH_LIBRARIES ${libnpd})
endif (libnpd)

## [4] libregx

find_library (libregx regx
  PATHS ${casa_locations}
  PATH_SUFFIXES ${lib_locations}
  )

if (libregx)
  list (APPEND GLISH_LIBRARIES ${libregx})
endif (libregx)

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

mark_as_advanced (
  HAVE_GLISH
  GLISH_INCLUDES
  GLISH_LIBRARIES
  )
