# - Check for the presence of the PGPLOT library
#
# Defines the following variables:
#  HAVE_PGPLOT       = 
#  PGPLOT_LIBRARIES  = Path to all parts of the PGPLOT library
#  PGPLOT_INCLUDES   = Path to the PGPLOT header files

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (PGPLOT_INCLUDES
  cpgplot.h
  PATHS /usr/local /usr /sw
  PATH_SUFFIXES include/pgplot lib/pgplot
  )

## -----------------------------------------------------------------------------
## Check for the library

## [1] libpgplot

find_library (PGPLOT_libpgplot
  NAMES pgplot
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES pgplot
  )

if (PGPLOT_libpgplot)
  list (APPEND PGPLOT_LIBRARIES ${PGPLOT_libpgplot})
else (PGPLOT_libpgplot)
  message (SEND_ERROR "Unable to locate libpgplot!")
endif (PGPLOT_libpgplot)

## [2] libcpgplot

find_library (PGPLOT_libcpgplot
  NAMES cpgplot
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES pgplot
  )

if (PGPLOT_libcpgplot)
  list (APPEND PGPLOT_LIBRARIES ${PGPLOT_libcpgplot})
else (PGPLOT_libcpgplot)
  message (SEND_ERROR "Unable to locate libcpgplot!")
endif (PGPLOT_libcpgplot)

## [3] libXmPgplot

find_library (PGPLOT_libXmPgplot
  NAMES XmPgplot
  PATHS /usr/local/lib /usr/lib /lib /opt /sw/lib
  PATH_SUFFIXES pgplot casa/local/lib
  )

if (PGPLOT_libXmPgplot)
  list (APPEND PGPLOT_LIBRARIES ${PGPLOT_libXmPgplot})
else (PGPLOT_libXmPgplot)
  message (SEND_ERROR "Unable to locate libXmPgplot!")
endif (PGPLOT_libXmPgplot)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (PGPLOT_INCLUDES AND PGPLOT_LIBRARIES)
  SET (HAVE_PGPLOT TRUE)
ELSE (PGPLOT_INCLUDES AND PGPLOT_LIBRARIES)
  IF (NOT PGPLOT_FIND_QUIETLY)
    IF (NOT PGPLOT_INCLUDES)
      MESSAGE (STATUS "Unable to find PGPLOT header files!")
    ENDIF (NOT PGPLOT_INCLUDES)
    IF (NOT PGPLOT_LIBRARIES)
      MESSAGE (STATUS "Unable to find PGPLOT library files!")
    ENDIF (NOT PGPLOT_LIBRARIES)
  ENDIF (NOT PGPLOT_FIND_QUIETLY)
ENDIF (PGPLOT_INCLUDES AND PGPLOT_LIBRARIES)

if (HAVE_PGPLOT)
  if (NOT PGPLOT_FIND_QUIETLY)
    message (STATUS "Found components for PGPLOT")
    message (STATUS "PGPLOT_INCLUDES  = ${PGPLOT_INCLUDES}")
    message (STATUS "PGPLOT_LIBRARIES = ${PGPLOT_LIBRARIES}")
  endif (NOT PGPLOT_FIND_QUIETLY)
else (HAVE_PGPLOT)
  if (PGPLOT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PGPLOT!")
  endif (PGPLOT_FIND_REQUIRED)
endif (HAVE_PGPLOT)

## -----------------------------------------------------------------------------

mark_as_advanced (
  HAVE_PGPLOT
  PGPLOT_INCLUDES
  PGPLOT_LIBRARIES
  )
