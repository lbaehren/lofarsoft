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

# - Check for the presence of Matlab
#
# The following variables are set when MATLAB is found:
#  HAVE_MATLAB       = Set to true, if all components of MATLAB
#                          have been found.
#  MATLAB_INCLUDES   = Include path for the header files of MATLAB
#  MATLAB_LIBRARIES  = Link these to use MATLAB
#  MATLAB_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (MATLAB_INCLUDES mat.h
  PATHS ${include_locations}
  PATH_SUFFIXES matlab
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (MATLAB_LIBRARIES matlab
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MATLAB_INCLUDES AND MATLAB_LIBRARIES)
  set (HAVE_MATLAB TRUE)
else (MATLAB_INCLUDES AND MATLAB_LIBRARIES)
  set (HAVE_MATLAB FALSE)
  if (NOT MATLAB_FIND_QUIETLY)
    if (NOT MATLAB_INCLUDES)
      message (STATUS "Unable to find Matlab header files!")
    endif (NOT MATLAB_INCLUDES)
    if (NOT MATLAB_LIBRARIES)
      message (STATUS "Unable to find Matlab library files!")
    endif (NOT MATLAB_LIBRARIES)
  endif (NOT MATLAB_FIND_QUIETLY)
endif (MATLAB_INCLUDES AND MATLAB_LIBRARIES)

if (HAVE_MATLAB)
  if (NOT MATLAB_FIND_QUIETLY)
    message (STATUS "Found components for Matlab")
    message (STATUS "MATLAB_INCLUDES  = ${MATLAB_INCLUDES}")
    message (STATUS "MATLAB_LIBRARIES = ${MATLAB_LIBRARIES}")
  endif (NOT MATLAB_FIND_QUIETLY)
else (HAVE_MATLAB)
  if (MATLAB_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Matlab!")
  endif (MATLAB_FIND_REQUIRED)
endif (HAVE_MATLAB)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MATLAB_INCLUDES
  MATLAB_LIBRARIES
  )
