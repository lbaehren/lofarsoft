# - Check for the presence of <PACKAGE>
#
# The following variables are set when <PACKAGE> is found:
#  HAVE_<PACKAGE>       = Set to true, if all components of <PACKAGE>
#                          have been found.
#  <PACKAGE>_INCLUDE_DIR = Include path for the header files of <PACKAGE>
#  <PACKAGE>_LIBRARY     = Link these to use <PACKAGE>

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (<PACKAGE>_INCLUDE_DIR <header file(s)>
  PATHS /usr/local/include /usr/include /sw/include
  PATH_SUFFIXES <optional path extension>
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (<PACKAGE>_LIBRARY <package name>
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (<PACKAGE>_INCLUDE_DIR AND <PACKAGE>_LIBRARY)
  SET (HAVE_<PACKAGE> TRUE)
ELSE (<PACKAGE>_INCLUDE_DIR AND <PACKAGE>_LIBRARY)
  IF (NOT <PACKAGE>_FIND_QUIETLY)
    IF (NOT <PACKAGE>_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find <PACKAGE> header files!")
    ENDIF (NOT <PACKAGE>_INCLUDE_DIR)
    IF (NOT <PACKAGE>_LIBRARY)
      MESSAGE (STATUS "Unable to find <PACKAGE> library files!")
    ENDIF (NOT <PACKAGE>_LIBRARY)
  ENDIF (NOT <PACKAGE>_FIND_QUIETLY)
ENDIF (<PACKAGE>_INCLUDE_DIR AND <PACKAGE>_LIBRARY)

IF (HAVE_<PACKAGE>)
  IF (NOT <PACKAGE>_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for <PACKAGE>")
    MESSAGE (STATUS "<PACKAGE>_INCLUDE_DIR = ${<PACKAGE>_INCLUDE_DIR}")
    MESSAGE (STATUS "<PACKAGE>_LIBRARY     = ${<PACKAGE>_LIBRARY}")
  ENDIF (NOT <PACKAGE>_FIND_QUIETLY)
ELSE (HAVE_<PACKAGE>)
  IF (<PACKAGE>_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find <PACKAGE>!")
  ENDIF (<PACKAGE>_FIND_REQUIRED)
ENDIF (HAVE_<PACKAGE>)

MARK_AS_ADVANCED (
  HAVE_<PACKAGE>
  <PACKAGE>_LIBRARY
  <PACKAGE>_INCLUDE_DIR
  )
