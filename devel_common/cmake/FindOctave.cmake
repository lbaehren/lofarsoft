# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2011                                                        |
# |   Lars B"ahren (lbaehren@gmail.com)                                         |
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

# - Check for the presence of OCTAVE
#
# The following variables are set when OCTAVE is found:
#  OCTAVE_FOUND      = Set to true, if all components of OCTAVE have been found.
#  OCTAVE_INCLUDES   = Include path for the header files of OCTAVE
#  OCTAVE_LIBRARIES  = Link these to use OCTAVE
#  OCTAVE_LFLAGS     = Linker flags (optional)

if (NOT OCTAVE_FOUND)

  if (NOT OCTAVE_ROOT_DIR)
    set (OCTAVE_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT OCTAVE_ROOT_DIR)
  
  set (OCTAVE_VERSIONS
    octave2.9
    octave-2.9
    octave-2.9.9
    octave2.1
    octave-2.1.73
    octave-2.1.73
    )
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (OCTAVE_INCLUDES octave/oct.h
    HINTS ${OCTAVE_ROOT_DIR}
    PATH_SUFFIXES ${OCTAVE_VERSIONS}
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (OCTAVE_LIBRARIES octave
    HINTS ${OCTAVE_ROOT_DIR}
    PATH_SUFFIXES ${OCTAVE_VERSIONS}
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)
    set (OCTAVE_FOUND TRUE)
  else (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)
    set (OCTAVE_FOUND FALSE)
    if (NOT OCTAVE_FIND_QUIETLY)
      if (NOT OCTAVE_INCLUDES)
	message (STATUS "Unable to find OCTAVE header files!")
      endif (NOT OCTAVE_INCLUDES)
      if (NOT OCTAVE_LIBRARIES)
	message (STATUS "Unable to find OCTAVE library files!")
      endif (NOT OCTAVE_LIBRARIES)
    endif (NOT OCTAVE_FIND_QUIETLY)
  endif (OCTAVE_INCLUDES AND OCTAVE_LIBRARIES)
  
  if (OCTAVE_FOUND)
    if (NOT OCTAVE_FIND_QUIETLY)
      message (STATUS "Found components for OCTAVE")
      message (STATUS "OCTAVE_ROOT_DIR  = ${OCTAVE_ROOT_DIR}")
      message (STATUS "OCTAVE_INCLUDES  = ${OCTAVE_INCLUDES}")
      message (STATUS "OCTAVE_LIBRARIES = ${OCTAVE_LIBRARIES}")
    endif (NOT OCTAVE_FIND_QUIETLY)
  else (OCTAVE_FOUND)
    if (OCTAVE_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find OCTAVE!")
    endif (OCTAVE_FIND_REQUIRED)
  endif (OCTAVE_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    OCTAVE_INCLUDES
    OCTAVE_LIBRARIES
    )
  
endif (NOT OCTAVE_FOUND)
