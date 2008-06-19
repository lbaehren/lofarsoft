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

# - Check for the presence of GLIB
#
# The following variables are set when GLIB is found:
#  HAVE_GLIB       = Set to true, if all components of GLIB have been found.
#  GLIB_INCLUDES   = Include path for the header files of GLIB
#  GLIB_LIBRARIES  = Link these to use GLIB
#  GLIB_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

## [1] find the main header file

find_path (GLIB_INCLUDES glib.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  glib
  glib/include
  glib-2.0
  glib-2.0/glib
  glib-2.0/include
  NO_DEFAULT_PATH
  )

## adjust the include path

string (REGEX REPLACE "include/glib-2.0/glib" "include/glib-2.0" GLIB_INCLUDES ${GLIB_INCLUDES})

## [2] further header files might be in a slightly different location

find_path (glibconfig_h glibconfig.h
  PATHS ${lib_locations}
  PATH_SUFFIXES
  glib
  glib/include
  glib-2.0
  glib-2.0/glib
  glib-2.0/include
  NO_DEFAULT_PATH
  )

if (glibconfig_h)
  message (STATUS "glibconfig.h = ${glibconfig_h}")
  list (APPEND GLIB_INCLUDES ${glibconfig_h})
endif (glibconfig_h)

## -----------------------------------------------------------------------------
## Check for the library

find_library (GLIB_LIBRARIES glib
  PATHS ${lib_locations}
  PATH_SUFFIXES
  glib
  glib/lib
  glib-2.0
  glib-2.0/glib
  glib-2.0/lib
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GLIB_INCLUDES AND GLIB_LIBRARIES)
  set (HAVE_GLIB TRUE)
else (GLIB_INCLUDES AND GLIB_LIBRARIES)
  set (HAVE_GLIB FALSE)
  if (NOT GLIB_FIND_QUIETLY)
    if (NOT GLIB_INCLUDES)
      message (STATUS "Unable to find Glib header files!")
    endif (NOT GLIB_INCLUDES)
    if (NOT GLIB_LIBRARIES)
      message (STATUS "Unable to find Glib library files!")
    endif (NOT GLIB_LIBRARIES)
  endif (NOT GLIB_FIND_QUIETLY)
endif (GLIB_INCLUDES AND GLIB_LIBRARIES)

if (HAVE_GLIB)
  if (NOT GLIB_FIND_QUIETLY)
    message (STATUS "Found components for Glib")
    message (STATUS "GLIB_INCLUDES  = ${GLIB_INCLUDES}")
    message (STATUS "GLIB_LIBRARIES = ${GLIB_LIBRARIES}")
  endif (NOT GLIB_FIND_QUIETLY)
else (HAVE_GLIB)
  if (GLIB_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GLIB!")
  endif (GLIB_FIND_REQUIRED)
endif (HAVE_GLIB)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GLIB_INCLUDES
  GLIB_LIBRARIES
  )
