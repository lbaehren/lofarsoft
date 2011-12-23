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

# - Check for the presence of STARTOOLS
#
# The following variables are set when STARTOOLS is found:
#  STARTOOLS_FOUND      = Set to true, if all components of STARTOOLS have been
#                         found.
#  STARTOOLS_INCLUDES   = Include path for the header files of STARTOOLS
#  STARTOOLS_LIBRARIES  = Link these to use STARTOOLS
#  STARTOOLS_LFLAGS     = Linker flags (optional)

if (NOT STARTOOLS_FOUND)

  if (NOT STARTOOLS_ROOT_DIR)
    set (STARTOOLS_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT STARTOOLS_ROOT_DIR)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (STARTOOLS_INCLUDES startools/util.hh startools/trigger.hh
    HINTS ${STARTOOLS_ROOT_DIR}
    PATH_SUFFIXES include
    )
  
  ##_____________________________________________________________________________
  ## Check for the library

  set (STARTOOLS_LIBRARIES "")

  ## libstarutil
  find_library (STARTOOLS_STARUTIL_LIBRARY starutil
    HINTS ${STARTOOLS_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  if (STARTOOLS_STARUTIL_LIBRARY)
    list (APPEND STARTOOLS_LIBRARIES ${STARTOOLS_STARUTIL_LIBRARY})
  endif (STARTOOLS_STARUTIL_LIBRARY)

  ## librecradio
  find_library (STARTOOLS_RECRADIO_LIBRARY recradio
    HINTS ${STARTOOLS_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  if (STARTOOLS_RECRADIO_LIBRARY)
    list (APPEND STARTOOLS_LIBRARIES ${STARTOOLS_RECRADIO_LIBRARY})
  endif (STARTOOLS_RECRADIO_LIBRARY)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)
    set (STARTOOLS_FOUND TRUE)
  else (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)
    set (STARTOOLS_FOUND FALSE)
    if (NOT STARTOOLS_FIND_QUIETLY)
      if (NOT STARTOOLS_INCLUDES)
	message (STATUS "Unable to find STARTOOLS header files!")
      endif (NOT STARTOOLS_INCLUDES)
      if (NOT STARTOOLS_LIBRARIES)
	message (STATUS "Unable to find STARTOOLS library files!")
      endif (NOT STARTOOLS_LIBRARIES)
    endif (NOT STARTOOLS_FIND_QUIETLY)
  endif (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)
  
  if (STARTOOLS_FOUND)
    if (NOT STARTOOLS_FIND_QUIETLY)
      message (STATUS "Found components for STARTOOLS")
      message (STATUS "STARTOOLS_ROOT_DIR  = ${STARTOOLS_ROOT_DIR}")
      message (STATUS "STARTOOLS_INCLUDES  = ${STARTOOLS_INCLUDES}")
      message (STATUS "STARTOOLS_LIBRARIES = ${STARTOOLS_LIBRARIES}")
    endif (NOT STARTOOLS_FIND_QUIETLY)
  else (STARTOOLS_FOUND)
    if (STARTOOLS_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find STARTOOLS!")
    endif (STARTOOLS_FIND_REQUIRED)
  endif (STARTOOLS_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    STARTOOLS_INCLUDES
    STARTOOLS_LIBRARIES
    )
  
endif (NOT STARTOOLS_FOUND)
