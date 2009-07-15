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

# - Check for the presence of IT++
#
# The following variables are set when IT++ is found:
#  HAVE_ITPP       = Set to true, if all components of IT++ have been found.
#  ITPP_INCLUDES   = Include path for the header files of IT++
#  ITPP_LIBRARIES  = Link these to use IT++
#  ITPP_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (ITPP_INCLUDES itbase.h itcomm.h
  PATHS ${include_locations}
  PATH_SUFFIXES itpp
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (ITPP_LIBRARIES itpp
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (ITPP_INCLUDES AND ITPP_LIBRARIES)
  set (HAVE_ITPP TRUE)
else (ITPP_INCLUDES AND ITPP_LIBRARIES)
  set (HAVE_ITPP FALSE)
  if (NOT ITPP_FIND_QUIETLY)
    if (NOT ITPP_INCLUDES)
      message (STATUS "Unable to find ITPP header files!")
    endif (NOT ITPP_INCLUDES)
    if (NOT ITPP_LIBRARIES)
      message (STATUS "Unable to find ITPP library files!")
    endif (NOT ITPP_LIBRARIES)
  endif (NOT ITPP_FIND_QUIETLY)
endif (ITPP_INCLUDES AND ITPP_LIBRARIES)

if (HAVE_ITPP)
  if (NOT ITPP_FIND_QUIETLY)
    message (STATUS "Found components for ITPP")
    message (STATUS "ITPP_INCLUDES  = ${ITPP_INCLUDES}")
    message (STATUS "ITPP_LIBRARIES = ${ITPP_LIBRARIES}")
  endif (NOT ITPP_FIND_QUIETLY)
else (HAVE_ITPP)
  if (ITPP_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find ITPP!")
  endif (ITPP_FIND_REQUIRED)
endif (HAVE_ITPP)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  ITPP_INCLUDES
  ITPP_LIBRARIES
  )
