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

# - Check for the presence of LIBXML2
#
# The following variables are set when LIBXML2 is found:
#  LIBXML2_FOUND      = Set to true, if all components of LIBXML2
#                         have been found.
#  LIBXML2_INCLUDES   = Include path for the header files of LIBXML2
#  LIBXML2_LIBRARIES  = Link these to use LIBXML2
#  LIBXML2_LFLAGS     = Linker flags (optional)

if (NOT LIBXML2_FOUND)

  ## Include common CMake settings
  if (EXISTS LUS_CMakeSettings.cmake)
    include (LUS_CMakeSettings)
  endif (EXISTS LUS_CMakeSettings.cmake)
  
  if (NOT LIBXML2_ROOT_DIR)
    set (LIBXML2_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT LIBXML2_ROOT_DIR)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (LIBXML2_INCLUDES xpath.h
    HINTS ${LIBXML2_ROOT_DIR}
    PATH_SUFFIXES libxml libxml2
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (LIBXML2_LIBRARIES xml2
    HINTS ${LIBXML2_ROOT_DIR}
    PATH_SUFFIXES libxml libxml2
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)
    set (LIBXML2_FOUND TRUE)
  else (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)
    set (LIBXML2_FOUND FALSE)
    if (NOT LIBXML2_FIND_QUIETLY)
      if (NOT LIBXML2_INCLUDES)
	message (STATUS "Unable to find LIBXML2 header files!")
      endif (NOT LIBXML2_INCLUDES)
      if (NOT LIBXML2_LIBRARIES)
	message (STATUS "Unable to find LIBXML2 library files!")
      endif (NOT LIBXML2_LIBRARIES)
    endif (NOT LIBXML2_FIND_QUIETLY)
  endif (LIBXML2_INCLUDES AND LIBXML2_LIBRARIES)
  
  if (LIBXML2_FOUND)
    if (NOT LIBXML2_FIND_QUIETLY)
      message (STATUS "Found components for LIBXML2")
      message (STATUS "LIBXML2_ROOT_DIR  = ${LIBXML2_ROOT_DIR}")
      message (STATUS "LIBXML2_INCLUDES  = ${LIBXML2_INCLUDES}")
      message (STATUS "LIBXML2_LIBRARIES = ${LIBXML2_LIBRARIES}")
    endif (NOT LIBXML2_FIND_QUIETLY)
  else (LIBXML2_FOUND)
    if (LIBXML2_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find LIBXML2!")
    endif (LIBXML2_FIND_REQUIRED)
  endif (LIBXML2_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    LIBXML2_INCLUDES
    LIBXML2_LIBRARIES
    )
  
endif (NOT LIBXML2_FOUND)
