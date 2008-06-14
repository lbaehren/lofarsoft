# +-----------------------------------------------------------------------------+
# | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                               $ |
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

# - Check for the presence of Numarray
#
# The following variables are set when Numarray is found:
#  HAVE_NUMARRAY       = Set to true, if all components of Numarray
#                          have been found.
#  NUMARRAY_INCLUDES   = Include path for the header files of Numarray
#  NUMARRAY_LIBRARIES  = Link these to use Numarray
#  NUMARRAY_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NUMARRAY_INCLUDES numarray.h libnumarray.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  python
  python/numarray
  python/numarray/Include/numarray/arrayobject.h
  python/site-packages
  python/site-packages/numpy
  python/site-packages/numpy/numarray
  NO_DEFAULT_PATH
  )

## most likely we need to adjust the path in order to support include via
## something like <numarray/numarray.h>

string (REGEX REPLACE "include/python/numarray" "include/python" NUMARRAY_INCLUDES ${NUMARRAY_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

find_library (NUMARRAY_LIBRARIES numarray
  PATHS ${lib_locations}
  PATH_SUFFIXES
  python
  python/numarray
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (NUMARRAY_INCLUDES AND NUMARRAY_LIBRARIES)
  set (HAVE_NUMARRAY TRUE)
else (NUMARRAY_INCLUDES AND NUMARRAY_LIBRARIES)
  set (HAVE_NUMARRAY FALSE)
  if (NOT NUMARRAY_FIND_QUIETLY)
    if (NOT NUMARRAY_INCLUDES)
      message (STATUS "Unable to find NUMARRAY header files!")
    endif (NOT NUMARRAY_INCLUDES)
    if (NOT NUMARRAY_LIBRARIES)
      message (STATUS "Unable to find NUMARRAY library files!")
    endif (NOT NUMARRAY_LIBRARIES)
  endif (NOT NUMARRAY_FIND_QUIETLY)
endif (NUMARRAY_INCLUDES AND NUMARRAY_LIBRARIES)

if (HAVE_NUMARRAY)
  if (NOT NUMARRAY_FIND_QUIETLY)
    message (STATUS "Found components for NUMARRAY")
    message (STATUS "NUMARRAY_INCLUDES  = ${NUMARRAY_INCLUDES}")
    message (STATUS "NUMARRAY_LIBRARIES = ${NUMARRAY_LIBRARIES}")
  endif (NOT NUMARRAY_FIND_QUIETLY)
else (HAVE_NUMARRAY)
  if (NUMARRAY_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NUMARRAY!")
  endif (NUMARRAY_FIND_REQUIRED)
endif (HAVE_NUMARRAY)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NUMARRAY_INCLUDES
  NUMARRAY_LIBRARIES
  )
