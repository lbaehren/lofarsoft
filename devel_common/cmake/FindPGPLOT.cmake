##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of the PGPLOT library
#
# Defines the following variables:
#  HAVE_PGPLOT       = 
#  PGPLOT_LIBRARY    = Path to libpgplot
#  CPGPLOT_LIBRARY   = Path to libcpgplot
#  PGPLOT_LIBRARIES  = Path to all parts of the PGPLOT library
#  PGPLOT_INCLUDES   = Path to the PGPLOT header files

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

FIND_PATH (PGPLOT_INCLUDES
  cpgplot.h
  PATHS ${include_locations}
  PATH_SUFFIXES include/pgplot lib/pgplot
  )

## -----------------------------------------------------------------------------
## Check for the library

## [1] libpgplot

find_library (PGPLOT_LIBRARY
  NAMES pgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (PGPLOT_LIBRARY)
  list (APPEND PGPLOT_LIBRARIES ${PGPLOT_LIBRARY})
else (PGPLOT_LIBRARY)
  message (SEND_ERROR "Unable to locate libpgplot!")
endif (PGPLOT_LIBRARY)

## [2] libcpgplot

find_library (CPGPLOT_LIBRARY
  NAMES cpgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (CPGPLOT_LIBRARY)
  list (APPEND PGPLOT_LIBRARIES ${CPGPLOT_LIBRARY})
else (CPGPLOT_LIBRARY)
  message (STATUS "Warning: Unable to locate libcpgplot!")
endif (CPGPLOT_LIBRARY)

## [3] libXmPgplot

find_library (PGPLOT_libXmPgplot
  NAMES XmPgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (PGPLOT_libXmPgplot)
  list (APPEND PGPLOT_LIBRARIES ${PGPLOT_libXmPgplot})
else (PGPLOT_libXmPgplot)
  message (STATUS "Warning: Unable to locate libXmPgplot!")
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
## Mark as advanced ...
