# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Martin van den Akker (martinva@astro.ru.nl)                               |
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

# - Check for the presence of AERA
#
# The following variables are set when AERA is found:
#  AERA_FOUND      = Set to true, if all components of AERA
#                          have been found.
#  AERA_INCLUDES   = Include path for the header files of AERA
#  AERA_LIBRARIES  = Link these to use AERA

if (NOT FIND_AERA_CMAKE)

  set (FIND_AERA_CMAKE TRUE)

  set (AERA_REQUIRED_VERSION "1.0.2")

  ##_____________________________________________________________________________
  ## Search locations

  include (CMakeSettings)

  ##_____________________________________________________________________________
  ## Check for the header files

  find_path (AERA_INCLUDES Datareader.h
    PATHS ${include_locations}
    PATH_SUFFIXES AERA
    )

  ##_____________________________________________________________________________
  ## Check for the library

  find_library (AERA_LIBRARIES AERADatareader
    PATHS ${lib_locations}
    )

  ##_____________________________________________________________________________
  ## Actions taken when all components have been found

  if (AERA_INCLUDES AND AERA_LIBRARIES)
    set (AERA_FOUND TRUE)
  else (AERA_INCLUDES AND AERA_LIBRARIES)
    set (AERA_FOUND FALSE)
    if (NOT AERA_FIND_QUIETLY)
      if (NOT AERA_INCLUDES)
	message (STATUS "Unable to find AERA header files!")
      endif (NOT AERA_INCLUDES)
      if (NOT AERA_LIBRARIES)
	message (STATUS "Unable to find AERA library files!")
      endif (NOT AERA_LIBRARIES)
    endif (NOT AERA_FIND_QUIETLY)
  endif (AERA_INCLUDES AND AERA_LIBRARIES)

  if (AERA_FOUND)
    if (NOT AERA_FIND_QUIETLY)
      message (STATUS "Found components for AERA")
      message (STATUS "AERA_INCLUDES  = ${AERA_INCLUDES}")
      message (STATUS "AERA_LIBRARIES = ${AERA_LIBRARIES}")
    endif (NOT AERA_FIND_QUIETLY)
  else (AERA_FOUND)
    if (AERA_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find AERA!")
    endif (AERA_FIND_REQUIRED)
  endif (AERA_FOUND)

  ##_____________________________________________________________________________
  ## Mark advanced variables

  mark_as_advanced (
    AERA_INCLUDES
    AERA_LIBRARIES
    )

endif (NOT FIND_AERA_CMAKE)
