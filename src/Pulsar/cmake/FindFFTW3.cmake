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

# - Check for the presence of FFTW3
#
# The following variables are set when FFTW3 is found:
#  FFTW3_FOUND      = Set to true, if all components of FFTW3
#                         have been found.
#  FFTW3_INCLUDES   = Include path for the header files of FFTW3
#  FFTW3_LIBRARIES  = Link these to use FFTW3
#  FFTW3_LFLAGS     = Linker flags (optional)

if (NOT FFTW3_FOUND)

  if (NOT FFTW3_ROOT_DIR)
    set (FFTW3_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT FFTW3_ROOT_DIR)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (FFTW3_INCLUDES fftw3.h
    HINTS ${FFTW3_ROOT_DIR}
    PATH_SUFFIXES
    include
    include/fftw3
    fftw3
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  set (FFTW3_LIBRARIES "")

  ##______________________________________________
  ## libfftw3 (shared/static)

  find_library (FFTW3_FFTW3_LIBRARY fftw3
    HINTS ${FFTW3_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  if (FFTW3_FFTW3_LIBRARY)
    list (APPEND FFTW3_LIBRARIES ${FFTW3_FFTW3_LIBRARY})
  endif (FFTW3_FFTW3_LIBRARY)

  ##______________________________________________
  ## libfftw3 (static)
  
  find_library (FFTW3_FFTW3_STATIC_LIBRARY ${CMAKE_FIND_LIBRARY_PREFIXES}fftw3.a
    HINTS ${FFTW3_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  
  ##______________________________________________
  ## libfftw3f (shared/static)
  
  find_library (FFTW3_FFTW3F_LIBRARY fftw3f
    HINTS ${FFTW3_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  if (FFTW3_FFTW3F_LIBRARY)
    list (APPEND FFTW3_LIBRARIES ${FFTW3_FFTW3F_LIBRARY})
  endif (FFTW3_FFTW3F_LIBRARY)
  
  ##______________________________________________
  ## libfftw3f (static)
  
  find_library (FFTW3_FFTW3F_STATIC_LIBRARY ${CMAKE_FIND_LIBRARY_PREFIXES}fftw3f.a
    HINTS ${FFTW3_ROOT_DIR}
    PATH_SUFFIXES lib
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
    set (FFTW3_FOUND TRUE)
  else (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
    set (FFTW3_FOUND FALSE)
    if (NOT FFTW3_FIND_QUIETLY)
      if (NOT FFTW3_INCLUDES)
	message (STATUS "Unable to find FFTW3 header files!")
      endif (NOT FFTW3_INCLUDES)
      if (NOT FFTW3_LIBRARIES)
	message (STATUS "Unable to find FFTW3 library files!")
      endif (NOT FFTW3_LIBRARIES)
    endif (NOT FFTW3_FIND_QUIETLY)
  endif (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
  
  if (FFTW3_FOUND)
    if (NOT FFTW3_FIND_QUIETLY)
      message (STATUS "Found components for FFTW3")
      message (STATUS "FFTW3_ROOT_DIR  = ${FFTW3_ROOT_DIR}")
      message (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
      message (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
    endif (NOT FFTW3_FIND_QUIETLY)
  else (FFTW3_FOUND)
    if (FFTW3_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find FFTW3!")
    endif (FFTW3_FIND_REQUIRED)
  endif (FFTW3_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    FFTW3_INCLUDES
    FFTW3_LIBRARIES
    )
  
endif (NOT FFTW3_FOUND)
