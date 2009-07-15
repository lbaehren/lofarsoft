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

# - Check for the presence of TCL
#
# The following variables are set when TCL is found:
#  HAVE_TCL       = Set to true, if all components of TCL have been found.
#  TCL_INCLUDES   = Include path for the header files of TCL
#  TCL_LIBRARIES  = Link these to use TCL
#  TCL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (TCL_INCLUDES tcl.h
  PATHS ${include_locations}
  PATH_SUFFIXES tcl tcl8.4 tcl8.3
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (TCL_LIBRARIES tcl tcl8.4 tcl8.3
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (TCL_INCLUDES AND TCL_LIBRARIES)
  set (HAVE_TCL TRUE)
else (TCL_INCLUDES AND TCL_LIBRARIES)
  set (HAVE_TCL FALSE)
  if (NOT TCL_FIND_QUIETLY)
    if (NOT TCL_INCLUDES)
      message (STATUS "Unable to find TCL header files!")
    endif (NOT TCL_INCLUDES)
    if (NOT TCL_LIBRARIES)
      message (STATUS "Unable to find TCL library files!")
    endif (NOT TCL_LIBRARIES)
  endif (NOT TCL_FIND_QUIETLY)
endif (TCL_INCLUDES AND TCL_LIBRARIES)

if (HAVE_TCL)
  if (NOT TCL_FIND_QUIETLY)
    message (STATUS "Found components for TCL")
    message (STATUS "TCL_INCLUDES  = ${TCL_INCLUDES}")
    message (STATUS "TCL_LIBRARIES = ${TCL_LIBRARIES}")
  endif (NOT TCL_FIND_QUIETLY)
else (HAVE_TCL)
  if (TCL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find TCL!")
  endif (TCL_FIND_REQUIRED)
endif (HAVE_TCL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  TCL_INCLUDES
  TCL_LIBRARIES
  )
