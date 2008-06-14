# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

# - Check for the presence of the Blitz++ library
#
# The following variables are set when Blitz++ is found:
#  HAVE_BLITZ      = True when both Blitz++ header and include files are 
#                    found; if any of them is missing, HAVE_BLITZ=false
#  BLITZ_INCLUDES  = the path to where the Blitz++ include files are.
#  BLITZ_LIBRARIES = Link these to use Blitz++

#SET (BLITZ_VERSION "1_33_1")

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files; the include statement in the code should be e.g.
##   #include <blitz/array.h>
## in order to ease differentiation from where the various header files are
## included.

FIND_PATH (BLITZ_INCLUDES blitz.h
  PATHS ${include_locations}
  PATH_SUFFIXES blitz
  )

IF (BLITZ_INCLUDES)
  STRING (REGEX REPLACE include/blitz include BLITZ_INCLUDES ${BLITZ_INCLUDES})
ENDIF (BLITZ_INCLUDES)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (BLITZ_LIBRARIES blitz
  PATHS ${lib_locations}
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BLITZ_INCLUDES AND BLITZ_LIBRARIES)
  SET (HAVE_BLITZ TRUE)
ELSE (BLITZ_INCLUDES AND BLITZ_LIBRARIES)
  SET (HAVE_BLITZ FALSE)
  IF (NOT BLITZ_FIND_QUIETLY)
    IF (NOT BLITZ_INCLUDES)
      MESSAGE (STATUS "Unable to find Blitz header files!")
    ENDIF (NOT BLITZ_INCLUDES)
    IF (NOT BLITZ_LIBRARIES)
      MESSAGE (STATUS "Unable to find Blitz library files!")
    ENDIF (NOT BLITZ_LIBRARIES)
  ENDIF (NOT BLITZ_FIND_QUIETLY)
ENDIF (BLITZ_INCLUDES AND BLITZ_LIBRARIES)

IF (HAVE_BLITZ)
  IF (NOT BLITZ_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Blitz")
    MESSAGE (STATUS "BLITZ_INCLUDES  = ${BLITZ_INCLUDES}")
    MESSAGE (STATUS "BLITZ_LIBRARIES = ${BLITZ_LIBRARIES}")
  ENDIF (NOT BLITZ_FIND_QUIETLY)
ELSE (HAVE_BLITZ)
  IF (BLITZ_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Blitz!")
  ENDIF (BLITZ_FIND_REQUIRED)
ENDIF (HAVE_BLITZ)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  BLITZ_INCLUDES
  BLITZ_LIBRARIES
  )
