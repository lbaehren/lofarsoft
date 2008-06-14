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

#
# this module look for libxml (http://www.xmlsoft.org) support
# it will define the following values
#
# LIBXML2_INCLUDES  = where libxml/xpath.h can be found
# LIBXML2_LIBRARIES      = the library to link against libxml2
# FOUND_LIBXML2        = set to 1 if libxml2 is found
#

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (LIBXML2_INCLUDES xpath.h
  PATHS ${include_locations}
  PATH_SUFFIXES libxml libxml2
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (LIBXML2_LIBRARIES xml2
  PATHS ${lib_locations}
  PATH_SUFFIXES libxml libxml2
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)
  SET (HAVE_LIBXML2 TRUE)
ELSE (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)
  SET (HAVE_LIBXML2 FALSE)
  IF (NOT LIBXML2_FIND_QUIETLY)
    IF (NOT LIBXML2_INCLUDES)
      MESSAGE (STATUS "Unable to find LIBXML2 header files!")
    ENDIF (NOT LIBXML2_INCLUDES)
    IF (NOT LIBXML2_LIBRARIES)
      MESSAGE (STATUS "Unable to find LIBXML2 library files!")
    ENDIF (NOT LIBXML2_LIBRARIES)
  ENDIF (NOT LIBXML2_FIND_QUIETLY)
ENDIF (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)

IF (HAVE_LIBXML2)
  IF (NOT LIBXML2_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LIBXML2")
    MESSAGE (STATUS "LIBXML2_INCLUDES  = ${LIBXML2_INCLUDES}")
    MESSAGE (STATUS "LIBXML2_LIBRARIES = ${LIBXML2_LIBRARIES}")
  ENDIF (NOT LIBXML2_FIND_QUIETLY)
ELSE (HAVE_LIBXML2)
  IF (LIBXML2_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LIBXML2!")
  ENDIF (LIBXML2_FIND_REQUIRED)
ENDIF (HAVE_LIBXML2)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  LIBXML2_INCLUDES
  LIBXML2_LIBRARIES
  )
