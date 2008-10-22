# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
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

# - Check for the presence of Octave
#
# The following variables are set when Octave is found:
#  HAVE_OCTAVE       = Set to true, if all components of Octave
#                          have been found.
#  OCTAVE_INCLUDES   = Include path for the header files of Octave
#  OCTAVE_LIBRARIES  = Link these to use Octave
#  OCTAVE_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (OCTAVE_INCLUDES octave/oct.h
  PATHS ${include_locations}
  PATH_SUFFIXES octave2.9 octave-2.9 octave-2.9.9 octave2.1 octave-2.1.73 octave-2.1.73
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (OCTAVE_LIBRARIES octave
  PATHS ${lib_locations}
  PATH_SUFFIXES octave2.9 octave-2.9 octave-2.9.9 octave2.1 octave-2.1.73 octave-2.1.73
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)
  set (HAVE_OCTAVE TRUE)
else (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)
  set (HAVE_OCTAVE FALSE)
  if (NOT OCTAVE_FIND_QUIETLY)
    if (NOT OCTAVE_INCLUDES)
      message (STATUS "Unable to find Octave header files!")
    endif (NOT OCTAVE_INCLUDES)
    if (NOT OCTAVE_LIBRARIES)
      message (STATUS "Unable to find Octave library files!")
    endif (NOT OCTAVE_LIBRARIES)
  endif (NOT OCTAVE_FIND_QUIETLY)
endif (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)

if (HAVE_OCTAVE)
  if (NOT OCTAVE_FIND_QUIETLY)
    message (STATUS "Found components for Octave")
    message (STATUS "OCTAVE_INCLUDES  = ${OCTAVE_INCLUDES}")
    message (STATUS "OCTAVE_LIBRARIES = ${OCTAVE_LIBRARIES}")
  endif (NOT OCTAVE_FIND_QUIETLY)
else (HAVE_OCTAVE)
  if (OCTAVE_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Octave!")
  endif (OCTAVE_FIND_REQUIRED)
endif (HAVE_OCTAVE)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  OCTAVE_INCLUDES
  OCTAVE_LIBRARIES
  )
