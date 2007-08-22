##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of POPT
#
# The following variables are set when POPT is found:
#  HAVE_POPT       = Set to true, if all components of POPT
#                          have been found.
#  POPT_INCLUDES = Include path for the header files of POPT
#  POPT_LIBRARIES     = Link these to use POPT

## -----------------------------------------------------------------------------
## Search locations

set (include_locations
  ../release/include
  ../../release/include
  /usr/include
  /usr/local/include
  /sw/include
  /opt
  /opt/include
)

set (lib_locations
  ../release/lib
  ../../release/lib
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt
  /opt/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (POPT_INCLUDES popt.h
  PATHS ${include_locations}
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (POPT_LIBRARIES popt
  PATHS ${lib_locations}
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (POPT_INCLUDES AND POPT_LIBRARIES)
  SET (HAVE_POPT TRUE)
ELSE (POPT_INCLUDES AND POPT_LIBRARIES)
  IF (NOT POPT_FIND_QUIETLY)
    IF (NOT POPT_INCLUDES)
      MESSAGE (STATUS "Unable to find POPT header files!")
    ENDIF (NOT POPT_INCLUDES)
    IF (NOT POPT_LIBRARIES)
      MESSAGE (STATUS "Unable to find POPT library files!")
    ENDIF (NOT POPT_LIBRARIES)
  ENDIF (NOT POPT_FIND_QUIETLY)
ENDIF (POPT_INCLUDES AND POPT_LIBRARIES)

IF (HAVE_POPT)
  IF (NOT POPT_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for POPT")
    MESSAGE (STATUS "POPT_INCLUDES   = ${POPT_INCLUDES}")
    MESSAGE (STATUS "POPT_LIBRARIES  = ${POPT_LIBRARIES}")
  ENDIF (NOT POPT_FIND_QUIETLY)
ELSE (HAVE_POPT)
  IF (POPT_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find POPT!")
  ENDIF (POPT_FIND_REQUIRED)
ENDIF (HAVE_POPT)
