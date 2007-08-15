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
  code/aips/glish/include
  code/aips/glish/include/Glish
  code/aips/glish/include/Npd
  code/aips/glish/include/sos
  code/aips/glish/include/regx
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

## [1] Glish/glish.h

find_path (GLISH_INCLUDES glish.h
  PATHS ${casa_locations}
  PATH_SUFFIXES ${include_locations}
  )

## [2] sos/sos.h

find_path (sos_sos_h sos.h
  PATHS ${casa_locations}
  PATH_SUFFIXES ${include_locations}
  )

if (sos_sos_h)
  string (REPLACE glish/include/sos glish/include tmp ${sos_sos_h})
  list (APPEND GLISH_INCLUDES ${tmp})
endif (sos_sos_h)

## [2] Npd/npd.h

find_path (npd_npd_h npd.h
  PATHS ${casa_locations}
  PATH_SUFFIXES ${include_locations}
  )

if (npd_npd_h)
  string (REGEX REPLACE "glish/include/Npd" "glish/include" tmp ${npd_npd_h})
  list (APPEND GLISH_INCLUDES ${tmp})
endif (npd_npd_h)

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
  IF (NOT GLISH_FIND_QUIETLY)
    IF (NOT GLISH_INCLUDES)
      MESSAGE (STATUS "Unable to find Glish header files!")
    ENDIF (NOT GLISH_INCLUDES)
    IF (NOT GLISH_LIBRARIES)
      MESSAGE (STATUS "Unable to find Glish library files!")
    ENDIF (NOT GLISH_LIBRARIES)
  ENDIF (NOT GLISH_FIND_QUIETLY)
ENDIF (GLISH_INCLUDES AND GLISH_LIBRARIES)

IF (HAVE_GLISH)
  IF (NOT GLISH_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Glish")
    MESSAGE (STATUS "GLISH_INCLUDES  = ${GLISH_INCLUDES}")
    MESSAGE (STATUS "GLISH_LIBRARIES = ${GLISH_LIBRARIES}")
  ENDIF (NOT GLISH_FIND_QUIETLY)
ELSE (HAVE_GLISH)
  IF (GLISH_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Glish!")
  ENDIF (GLISH_FIND_REQUIRED)
ENDIF (HAVE_GLISH)

## -----------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  HAVE_GLISH
  GLISH_INCLUDES
  GLISH_LIBRARIES
  )
