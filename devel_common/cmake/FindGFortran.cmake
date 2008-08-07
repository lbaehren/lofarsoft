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

# - Check for the presence of GFORTRAN
#
# The following variables are set when GFORTRAN is found:
#  HAVE_GFORTRAN       = Set to true, if all components of GFORTRAN have been
#                        found.
#  GFORTRAN_INCLUDES   = Include path for the header files of GFORTRAN
#  GFORTRAN_LIBRARIES  = Link these to use GFORTRAN
#  GFORTRAN_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GFORTRAN_INCLUDES libgfortran.h
  PATHS ${include_locations}
  PATH_SUFFIXES libgfortran
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (HAVE_LIBGFORTRAN gfortran
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (HAVE_LIBGFORTRAN)
  set (GFORTRAN_LIBRARIES ${HAVE_LIBGFORTRAN})
endif (HAVE_LIBGFORTRAN)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARIES)
  set (HAVE_GFORTRAN TRUE)
else (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARIES)
  set (HAVE_GFORTRAN FALSE)
  if (NOT GFORTRAN_FIND_QUIETLY)
    if (NOT GFORTRAN_INCLUDES)
      message (STATUS "Unable to find GFortran header files!")
    endif (NOT GFORTRAN_INCLUDES)
    if (NOT GFORTRAN_LIBRARIES)
      message (STATUS "Unable to find GFortran library files!")
    endif (NOT GFORTRAN_LIBRARIES)
  endif (NOT GFORTRAN_FIND_QUIETLY)
endif (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARIES)

if (HAVE_GFORTRAN)
  if (NOT GFORTRAN_FIND_QUIETLY)
    message (STATUS "Found components for GFortran")
    message (STATUS "GFORTRAN_INCLUDES  = ${GFORTRAN_INCLUDES}")
    message (STATUS "GFORTRAN_LIBRARIES = ${GFORTRAN_LIBRARIES}")
  endif (NOT GFORTRAN_FIND_QUIETLY)
else (HAVE_GFORTRAN)
  if (GFORTRAN_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GFortran!")
  endif (GFORTRAN_FIND_REQUIRED)
endif (HAVE_GFORTRAN)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GFORTRAN_INCLUDES
  GFORTRAN_LIBRARIES
  HAVE_LIBGFORTRAN
  )
