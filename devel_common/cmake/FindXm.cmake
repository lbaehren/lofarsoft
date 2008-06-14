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

# - Check for the presence of XM
#
# The following variables are set when XM is found:
#  HAVE_XM       = Set to true, if all components of XM have been found.
#  XM_INCLUDES   = Include path for the header files of XM
#  XM_LIBRARIES  = Link these to use XM
#  XM_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (XM_INCLUDES Xm.h XmP.h
  PATHS ${include_locations}
  PATH_SUFFIXES Xm X11 X11/Xm
  NO_DEFAULT_PATH
  )

string (REGEX REPLACE "include/Xm" "include" XM_INCLUDES ${XM_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

find_library (XM_LIBRARIES Xm
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (XM_INCLUDES AND XM_LIBRARIES)
  set (HAVE_XM TRUE)
else (XM_INCLUDES AND XM_LIBRARIES)
  set (HAVE_XM FALSE)
  if (NOT XM_FIND_QUIETLY)
    if (NOT XM_INCLUDES)
      message (STATUS "Unable to find XM header files!")
    endif (NOT XM_INCLUDES)
    if (NOT XM_LIBRARIES)
      message (STATUS "Unable to find XM library files!")
    endif (NOT XM_LIBRARIES)
  endif (NOT XM_FIND_QUIETLY)
endif (XM_INCLUDES AND XM_LIBRARIES)

if (HAVE_XM)
  if (NOT XM_FIND_QUIETLY)
    message (STATUS "Found components for XM")
    message (STATUS "XM_INCLUDES  = ${XM_INCLUDES}")
    message (STATUS "XM_LIBRARIES = ${XM_LIBRARIES}")
  endif (NOT XM_FIND_QUIETLY)
else (HAVE_XM)
  if (XM_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find XM!")
  endif (XM_FIND_REQUIRED)
endif (HAVE_XM)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  XM_INCLUDES
  XM_LIBRARIES
  )
