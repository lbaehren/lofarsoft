# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
# |   Alwin de Jong (jong@astron.nl)                                            |
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

# - Check for the presence of the LOFAR libraries and header files
#
# Defines the following variables:
#  HAVE_LOFAR_PARSET_READER   -- If LOFAR libraries and header files are found
#  LOFAR_COMMON_LIBRARY       -- Path to lofar libraries
#  LOFAR_PARSET_HEADERS       -- Directory containing the LOFAR RTCP parset reader header files

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)
INCLUDE(FindPackageHandleStandardArgs)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (LOFAR_COMMON_LIBRARY
  NAMES common
  HINTS ENV LOFARROOT
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH
)

FIND_LIBRARY (LOFAR_INTERFACE_LIBRARY
  NAMES interface
  HINTS ENV LOFARROOT
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (LOFAR_PARSET_HEADERS
  Parset.h
  HINTS ENV LOFARROOT
  PATH_SUFFIXES Interface
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LOFAR DEFAULT_MSG LOFAR_PARSET_HEADERS LOFAR_COMMON_LIBRARY LOFAR_INTERFACE_LIBRARY)

IF (LOFAR_FOUND)
  SET(LOFAR_INCLUDES ${LOFAR_PARSET_HEADERS})
  SET(LOFAR_LIBRARIES ${LOFAR_COMMON_LIBRARY} ${LOFAR_INTERFACE_LIBRARY})
  IF (NOT LOFAR_FIND_QUIETLY)
  MESSAGE (STATUS "Found components for LOFAR parset reader")
  MESSAGE (STATUS "LOFAR_COMMON_LIBRARY    = ${LOFAR_COMMON_LIBRARY}")
  MESSAGE (STATUS "LOFAR_INTERFACE_LIBRARY = ${LOFAR_INTERFACE_LIBRARY}")
  MESSAGE (STATUS "LOFAR_PARSET_HEADERS    = ${LOFAR_PARSET_HEADERS}")
  ENDIF (NOT LOFAR_FIND_QUIETLY)
ENDIF (LOFAR_FOUND)
