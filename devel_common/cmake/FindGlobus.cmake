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

# - Check for the presence of Globus
#
# The following variables are set when Globus is found:
#  HAVE_Globus       = Set to true, if all components of Globus
#                          have been found.
#  Globus_INCLUDES   = Include path for the header files of Globus
#  Globus_LIBRARIES  = Link these to use Globus

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (Globus_INCLUDES globus.h
  PATHS ${include_locations}
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (Globus_LIBRARIES globus
  PATHS ${lib_locations}
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (Globus_INCLUDES AND Globus_LIBRARIES)
  set (HAVE_Globus TRUE)
else (Globus_INCLUDES AND Globus_LIBRARIES)
  if (NOT Globus_FIND_QUIETLY)
    if (NOT Globus_INCLUDES)
      message (STATUS "Unable to find Globus header files!")
    endif (NOT Globus_INCLUDES)
    if (NOT Globus_LIBRARIES)
      message (STATUS "Unable to find Globus library files!")
    endif (NOT Globus_LIBRARIES)
  endif (NOT Globus_FIND_QUIETLY)
endif (Globus_INCLUDES AND Globus_LIBRARIES)

if (HAVE_Globus)
  if (NOT Globus_FIND_QUIETLY)
    message (STATUS "Found components for Globus")
    message (STATUS "Globus_INCLUDES  = ${Globus_INCLUDES}")
    message (STATUS "Globus_LIBRARIES = ${Globus_LIBRARIES}")
  endif (NOT Globus_FIND_QUIETLY)
else (HAVE_Globus)
  if (Globus_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Globus!")
  endif (Globus_FIND_REQUIRED)
endif (HAVE_Globus)

## -----------------------------------------------------------------------------
## Mark as advanced ...
