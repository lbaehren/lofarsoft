# - Check for the presence of POPT
#
# The following variables are set when POPT is found:
#  HAVE_POPT       = Set to true, if all components of POPT
#                          have been found.
#  POPT_INCLUDE_DIR = Include path for the header files of POPT
#  POPT_LIBRARY     = Link these to use POPT

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (POPT_INCLUDE_DIR popt.h
  PATHS /usr/local/include /usr/include /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (POPT_LIBRARY popt
  PATHS /lib /usr/lib /usr/local/lib /sw/lib /opt/casa/local/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (POPT_INCLUDE_DIR AND POPT_LIBRARY)
  SET (HAVE_POPT TRUE)
ELSE (POPT_INCLUDE_DIR AND POPT_LIBRARY)
  IF (NOT POPT_FIND_QUIETLY)
    IF (NOT POPT_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find POPT header files!")
    ENDIF (NOT POPT_INCLUDE_DIR)
    IF (NOT POPT_LIBRARY)
      MESSAGE (STATUS "Unable to find POPT library files!")
    ENDIF (NOT POPT_LIBRARY)
  ENDIF (NOT POPT_FIND_QUIETLY)
ENDIF (POPT_INCLUDE_DIR AND POPT_LIBRARY)

IF (HAVE_POPT)
  IF (NOT POPT_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for POPT")
    MESSAGE (STATUS "POPT_INCLUDE_DIR = ${POPT_INCLUDE_DIR}")
    MESSAGE (STATUS "POPT_LIBRARY     = ${POPT_LIBRARY}")
  ENDIF (NOT POPT_FIND_QUIETLY)
ELSE (HAVE_POPT)
  IF (POPT_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find POPT!")
  ENDIF (POPT_FIND_REQUIRED)
ENDIF (HAVE_POPT)

MARK_AS_ADVANCED (
  HAVE_POPT
  POPT_LIBRARY
  POPT_INCLUDE_DIR
  )
