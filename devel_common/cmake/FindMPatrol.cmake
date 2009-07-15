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

# - Check for the presence of mpatrol
#
# The following variables are set when mpatrol is found:
#  HAVE_MPATROL       = Set to true, if all components of MPATROL
#                          have been found.
#  MPATROL_INCLUDES   = Include path for the header files of MPATROL
#  MPATROL_LIBRARIES  = Link these to use MPATROL
#  MPATROL_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (MPATROL_INCLUDES mpatrol.h
  PATHS ${include_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (MPATROL_LIBRARIES mpatrol
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MPATROL_INCLUDES AND MPATROL_LIBRARIES)
  set (HAVE_MPATROL TRUE)
else (MPATROL_INCLUDES AND MPATROL_LIBRARIES)
  set (HAVE_MPATROL FALSE)
  if (NOT MPATROL_FIND_QUIETLY)
    if (NOT MPATROL_INCLUDES)
      message (STATUS "Unable to find mpatrol header files!")
    endif (NOT MPATROL_INCLUDES)
    if (NOT MPATROL_LIBRARIES)
      message (STATUS "Unable to find mpatrol library files!")
    endif (NOT MPATROL_LIBRARIES)
  endif (NOT MPATROL_FIND_QUIETLY)
endif (MPATROL_INCLUDES AND MPATROL_LIBRARIES)

if (HAVE_MPATROL)
  if (NOT MPATROL_FIND_QUIETLY)
    message (STATUS "Found components for mpatrol")
    message (STATUS "MPATROL_INCLUDES  = ${MPATROL_INCLUDES}")
    message (STATUS "MPATROL_LIBRARIES = ${MPATROL_LIBRARIES}")
  endif (NOT MPATROL_FIND_QUIETLY)
else (HAVE_MPATROL)
  if (MPATROL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find mpatrol!")
  endif (MPATROL_FIND_REQUIRED)
endif (HAVE_MPATROL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MPATROL_INCLUDES
  MPATROL_LIBRARIES
  )
