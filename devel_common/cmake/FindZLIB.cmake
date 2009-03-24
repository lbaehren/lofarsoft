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

# - Check for the presence of ZLIB
#
# The following variables are set when ZLIB is found:
#  HAVE_ZLIB       = Set to true, if all components of ZLIB have been found.
#  ZLIB_INCLUDES   = Include path for the header files of ZLIB
#  ZLIB_LIBRARIES  = Link these to use ZLIB
#  ZLIB_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (ZLIB_INCLUDES zlib.h zutil.h
  PATHS ${include_locations}
  PATH_SUFFIXES zlib
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (ZLIB_LIBRARIES z
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (ZLIB_INCLUDES AND ZLIB_LIBRARIES)
  set (HAVE_ZLIB TRUE)
else (ZLIB_INCLUDES AND ZLIB_LIBRARIES)
  set (HAVE_ZLIB FALSE)
  if (NOT ZLIB_FIND_QUIETLY)
    if (NOT ZLIB_INCLUDES)
      message (STATUS "Unable to find ZLIB header files!")
    endif (NOT ZLIB_INCLUDES)
    if (NOT ZLIB_LIBRARIES)
      message (STATUS "Unable to find ZLIB library files!")
    endif (NOT ZLIB_LIBRARIES)
  endif (NOT ZLIB_FIND_QUIETLY)
endif (ZLIB_INCLUDES AND ZLIB_LIBRARIES)

if (HAVE_ZLIB)
  if (NOT ZLIB_FIND_QUIETLY)
    message (STATUS "Found components for ZLIB")
    message (STATUS "ZLIB_INCLUDES  = ${ZLIB_INCLUDES}")
    message (STATUS "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")
  endif (NOT ZLIB_FIND_QUIETLY)
else (HAVE_ZLIB)
  if (ZLIB_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find ZLIB!")
  endif (ZLIB_FIND_REQUIRED)
endif (HAVE_ZLIB)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  ZLIB_INCLUDES
  ZLIB_LIBRARIES
  )
