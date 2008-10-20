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

# - Check for the presence of <PACKAGE>
#
# The following variables are set when MathGL is found:
#  HAVE_MATHGL       = Set to true, if all components of MathGL have been found.
#  MATHGL_INCLUDES   = Include path for the header files of MathGL
#  MATHGL_LIBRARIES  = Link these to use MathGL
#  MATHGL_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (MATHGL_INCLUDES mgl/mgl.h mgl/mgl_data.h
  PATHS ${include_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (MATHGL_LIBRARIES mgl mgl_hdf
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MATHGL_INCLUDES AND MATHGL_LIBRARIES)
  set (HAVE_MATHGL TRUE)
else (MATHGL_INCLUDES AND MATHGL_LIBRARIES)
  set (HAVE_MATHGL FALSE)
  if (NOT MATHGL_FIND_QUIETLY)
    if (NOT MATHGL_INCLUDES)
      message (STATUS "Unable to find MATHGL header files!")
    endif (NOT MATHGL_INCLUDES)
    if (NOT MATHGL_LIBRARIES)
      message (STATUS "Unable to find MATHGL library files!")
    endif (NOT MATHGL_LIBRARIES)
  endif (NOT MATHGL_FIND_QUIETLY)
endif (MATHGL_INCLUDES AND MATHGL_LIBRARIES)

if (HAVE_MATHGL)
  if (NOT MATHGL_FIND_QUIETLY)
    message (STATUS "Found components for MATHGL")
    message (STATUS "MATHGL_INCLUDES  = ${MATHGL_INCLUDES}")
    message (STATUS "MATHGL_LIBRARIES = ${MATHGL_LIBRARIES}")
  endif (NOT MATHGL_FIND_QUIETLY)
else (HAVE_MATHGL)
  if (MATHGL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find MATHGL!")
  endif (MATHGL_FIND_REQUIRED)
endif (HAVE_MATHGL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MATHGL_INCLUDES
  MATHGL_LIBRARIES
  )
