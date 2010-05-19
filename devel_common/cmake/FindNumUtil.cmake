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

# - Check for the presence of NUM_UTIL
#
# The following variables are set when NUM_UTIL is found:
#  HAVE_NUM_UTIL       = Set to true, if all components of NUM_UTIL
#                          have been found.
#  NUM_UTIL_INCLUDES   = Include path for the header files of NUM_UTIL
#  NUM_UTIL_LIBRARIES  = Link these to use NUM_UTIL
#  NUM_UTIL_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

if (NUM_UTIL_FIND_QUIETLY)
  set (PYTHON_FIND_QUIETLY TRUE)
endif (NUM_UTIL_FIND_QUIETLY)

include (FindPython)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NUM_UTIL_INCLUDES num_util.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  python
  num_util
  python/num_util
  python${PYTHON_VERSION}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (NUM_UTIL_LIBRARIES num_util
  PATHS ${lib_locations}
  PATH_SUFFIXES
  python
  python${PYTHON_VERSION}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

set (NUM_UTIL_FIND_QUIETLY ${NUMUTIL_FIND_QUIETLY})

if (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES)
  set (HAVE_NUM_UTIL TRUE)
else (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES)
  set (HAVE_NUM_UTIL FALSE)
  if (NOT NUM_UTIL_FIND_QUIETLY)
    if (NOT NUM_UTIL_INCLUDES)
      message (STATUS "Unable to find NUM_UTIL header files!")
    endif (NOT NUM_UTIL_INCLUDES)
    if (NOT NUM_UTIL_LIBRARIES)
      message (STATUS "Unable to find NUM_UTIL library files!")
    endif (NOT NUM_UTIL_LIBRARIES)
  endif (NOT NUM_UTIL_FIND_QUIETLY)
endif (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES)

if (HAVE_NUM_UTIL)
  if (NOT NUM_UTIL_FIND_QUIETLY)
    message (STATUS "Found components for NUM_UTIL")
    message (STATUS "NUM_UTIL_INCLUDES  = ${NUM_UTIL_INCLUDES}")
    message (STATUS "NUM_UTIL_LIBRARIES = ${NUM_UTIL_LIBRARIES}")
  endif (NOT NUM_UTIL_FIND_QUIETLY)
else (HAVE_NUM_UTIL)
  if (NUM_UTIL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NUM_UTIL!")
  endif (NUM_UTIL_FIND_REQUIRED)
endif (HAVE_NUM_UTIL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NUM_UTIL_INCLUDES
  NUM_UTIL_LIBRARIES
  )
