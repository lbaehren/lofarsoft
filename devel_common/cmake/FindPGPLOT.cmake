##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of the PGPLOT library
#
# Defines the following variables:
#  HAVE_PGPLOT       = 
#  PGPLOT_INCLUDES   = Path to the PGPLOT header files
#  PGPLOT_LIBRARIES  = Path to all parts of the PGPLOT library

## -----------------------------------------------------------------------------
## Search locations

set (include_locations
  ## local installation
  ../release/include
  ../../release/include
  ## system-wide installation
  /usr/include
  /usr/local/include
  /sw/include
  /opt
  /opt/include
)

set (lib_locations
  ## local installation
  ../release/lib
  ../../release/lib
  ## system-wide installation
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt
  /opt/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (PGPLOT_INCLUDES cpgplot.h
  PATHS ${include_locations}
  PATH_SUFFIXES pgplot
  )

## -----------------------------------------------------------------------------
## Check for the library

set (PGPLOT_LIBRARIES "")

## [1] libpgplot

find_library (libpgplot
  NAMES pgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (libpgplot)
  list (APPEND PGPLOT_LIBRARIES ${libpgplot})
else (libpgplot)
  message (STATUS "Warning: Unable to locate libpgplot!")
endif (libpgplot)

## [2] libcpgplot

find_library (libcpgplot
  NAMES cpgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (libcpgplot)
  list (APPEND PGPLOT_LIBRARIES ${libcpgplot})
else (libcpgplot)
  message (STATUS "Warning: Unable to locate libcpgplot!")
endif (libcpgplot)

## [3] libXmPgplot

find_library (libXmPgplot
  NAMES XmPgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
  NO_DEFAULT_PATH
  )

if (libXmPgplot)
  list (APPEND PGPLOT_LIBRARIES ${libXmPgplot})
else (libXmPgplot)
  message (STATUS "Warning: Unable to locate libXmPgplot!")
endif (libXmPgplot)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (PGPLOT_INCLUDES AND PGPLOT_LIBRARIES)
  SET (HAVE_PGPLOT TRUE)
ELSE (PGPLOT_INCLUDES AND PGPLOT_LIBRARIES)
  SET (HAVE_PGPLOT FALSE)
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

mark_as_advanced (
  PGPLOT_INCLUDES
  PGPLOT_LIBRARIES
  )