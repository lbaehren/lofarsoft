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

# - Check for the presence of PELICAN
#
# The following variables are set when PELICAN is found:
#  HAVE_PELICAN       = Set to true, if all components of PELICAN have been found.
#  PELICAN_INCLUDES   = Include path for the header files of PELICAN
#  PELICAN_LIBRARIES  = Link these to use PELICAN
#  PELICAN_LFLAGS     = Linker flags (optional)

if (NOT FIND_PELICAN_CMAKE)
  
  set (FIND_PELICAN_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (PELICAN_INCLUDES pelican/comms/ServerRequest.h
    PATHS ${include_locations}
    PATH_SUFFIXES pelican
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (PELICAN_LIBRARIES pelican
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (PELICAN_INCLUDES AND PELICAN_LIBRARIES)
    set (HAVE_PELICAN TRUE)
  else (PELICAN_INCLUDES AND PELICAN_LIBRARIES)
    set (HAVE_PELICAN FALSE)
    if (NOT PELICAN_FIND_QUIETLY)
      if (NOT PELICAN_INCLUDES)
	message (STATUS "Unable to find PELICAN header files!")
      endif (NOT PELICAN_INCLUDES)
      if (NOT PELICAN_LIBRARIES)
	message (STATUS "Unable to find PELICAN library files!")
      endif (NOT PELICAN_LIBRARIES)
    endif (NOT PELICAN_FIND_QUIETLY)
  endif (PELICAN_INCLUDES AND PELICAN_LIBRARIES)
  
  if (HAVE_PELICAN)
    if (NOT PELICAN_FIND_QUIETLY)
      message (STATUS "Found components for PELICAN")
      message (STATUS "PELICAN_INCLUDES  = ${PELICAN_INCLUDES}")
      message (STATUS "PELICAN_LIBRARIES = ${PELICAN_LIBRARIES}")
    endif (NOT PELICAN_FIND_QUIETLY)
  else (HAVE_PELICAN)
    if (PELICAN_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find PELICAN!")
    endif (PELICAN_FIND_REQUIRED)
  endif (HAVE_PELICAN)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    PELICAN_INCLUDES
    PELICAN_LIBRARIES
    )
  
endif (NOT FIND_PELICAN_CMAKE)
