# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
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

# - Check for the presence of PIL
#
# The following variables are set when PIL is found:
#  HAVE_PIL        = Set to true, if all components of PIL have been found.
#  PIL_INCLUDES    = Include path for the header files of PIL
#  PIL_LIBRARIES   = Link these to use PIL
#  PGET_EXECUTABLE = pget application executable
#  PIL_LFLAGS      = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PIL_INCLUDES pil.h
  PATHS ${include_locations}
  PATH_SUFFIXES pil
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (PIL_LIBRARIES pil
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the executables

find_program (PGET_EXECUTABLE pget
  PATHS ${bin_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PIL_INCLUDES AND PIL_LIBRARIES)
  set (HAVE_PIL TRUE)
else (PIL_INCLUDES AND PIL_LIBRARIES)
  set (HAVE_PIL FALSE)
  if (NOT PIL_FIND_QUIETLY)
    if (NOT PIL_INCLUDES)
      message (STATUS "Unable to find PIL header files!")
    endif (NOT PIL_INCLUDES)
    if (NOT PIL_LIBRARIES)
      message (STATUS "Unable to find PIL library files!")
    endif (NOT PIL_LIBRARIES)
  endif (NOT PIL_FIND_QUIETLY)
endif (PIL_INCLUDES AND PIL_LIBRARIES)

if (HAVE_PIL)
  if (NOT PIL_FIND_QUIETLY)
    message (STATUS "Found components for PIL")
    message (STATUS "PIL_INCLUDES  = ${PIL_INCLUDES}")
    message (STATUS "PIL_LIBRARIES = ${PIL_LIBRARIES}")
  endif (NOT PIL_FIND_QUIETLY)
else (HAVE_PIL)
  if (PIL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PIL!")
  endif (PIL_FIND_REQUIRED)
endif (HAVE_PIL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PIL_INCLUDES
  PIL_LIBRARIES
  )
