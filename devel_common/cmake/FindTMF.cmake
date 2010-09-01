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

# - Check for the presence of TMF
#
# The following variables are set when TMF is found:
#  HAVE_TMF       = Set to true, if all components of TMF have been found.
#  TMF_INCLUDES   = Include path for the header files of TMF
#  TMF_LIBRARIES  = Link these to use TMF

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (TMF_INCLUDES tmf.h
  PATHS
  ${include_locations}
  /opt/local/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (TMF_LIBRARIES tmf
  PATHS
  ${lib_locations}
  /opt/local/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (TMF_INCLUDES AND TMF_LIBRARIES)
  SET (HAVE_TMF TRUE)
ELSE (TMF_INCLUDES AND TMF_LIBRARIES)
  SET (HAVE_TMF FALSE)
  IF (NOT TMF_FIND_QUIETLY)
    IF (NOT TMF_INCLUDES)
      MESSAGE (STATUS "Unable to find TMF header files!")
    ENDIF (NOT TMF_INCLUDES)
    IF (NOT TMF_LIBRARIES)
      MESSAGE (STATUS "Unable to find TMF library files!")
    ENDIF (NOT TMF_LIBRARIES)
  ENDIF (NOT TMF_FIND_QUIETLY)
ENDIF (TMF_INCLUDES AND TMF_LIBRARIES)

IF (HAVE_TMF)
  IF (NOT TMF_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for TMF")
    MESSAGE (STATUS "TMF_INCLUDES  = ${TMF_INCLUDES}")
    MESSAGE (STATUS "TMF_LIBRARIES = ${TMF_LIBRARIES}")
  ENDIF (NOT TMF_FIND_QUIETLY)
ELSE (HAVE_TMF)
  IF (TMF_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find TMF!")
  ENDIF (TMF_FIND_REQUIRED)
ENDIF (HAVE_TMF)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  TMF_INCLUDES
  TMF_LIBRARIES
  )

