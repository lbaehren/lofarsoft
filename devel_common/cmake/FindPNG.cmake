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

# - Check for the presence of PNG
#
# The following variables are set when PNG is found:
#  HAVE_PNG       = Set to true, if all components of PNG have been found.
#  PNG_INCLUDES   = Include path for the header files of PNG
#  PNG_LIBRARIES  = Link these to use PNG
#  PNG_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PNG_INCLUDES png.h
  PATHS ${include_locations}
  PATH_SUFFIXES png
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (PNG_LIBRARIES png
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PNG_INCLUDES AND PNG_LIBRARIES)
  set (HAVE_PNG TRUE)
else (PNG_INCLUDES AND PNG_LIBRARIES)
  set (HAVE_PNG FALSE)
  if (NOT PNG_FIND_QUIETLY)
    if (NOT PNG_INCLUDES)
      message (STATUS "Unable to find PNG header files!")
    endif (NOT PNG_INCLUDES)
    if (NOT PNG_LIBRARIES)
      message (STATUS "Unable to find PNG library files!")
    endif (NOT PNG_LIBRARIES)
  endif (NOT PNG_FIND_QUIETLY)
endif (PNG_INCLUDES AND PNG_LIBRARIES)

if (HAVE_PNG)
  if (NOT PNG_FIND_QUIETLY)
    message (STATUS "Found components for PNG")
    message (STATUS "PNG_INCLUDES  = ${PNG_INCLUDES}")
    message (STATUS "PNG_LIBRARIES = ${PNG_LIBRARIES}")
  endif (NOT PNG_FIND_QUIETLY)
else (HAVE_PNG)
  if (PNG_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PNG!")
  endif (PNG_FIND_REQUIRED)
endif (HAVE_PNG)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PNG_INCLUDES
  PNG_LIBRARIES
  )
