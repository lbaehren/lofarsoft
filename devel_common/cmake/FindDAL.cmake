# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2012                                                        |
# |   Pim Schellart (P.Schellart@astro.ru.nl)                                   |
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

# - Check for the presence of DAL
#
# The following variables are set when DAL is found:
#
#   DAL_FOUND      = Set to true, if all components of DAL have been found.
#   DAL_INCLUDES   = Include path for the header files of DAL
#   DAL_LIBRARIES  = Link these to use DAL
#   DAL_VERSION_MAJOR = Major version number
#   DAL_VERSION_MINOR = Minor version number
#   DAL_VERSION_PATCH = Patch version number
#   DAL_VERSION       = DAL version string

if (NOT DAL_FOUND)

  if (NOT DAL_ROOT_DIR)
    set (DAL_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT DAL_ROOT_DIR)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (DAL_INCLUDES dal_config.h
    HINTS ${DAL_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${include_locations}
    PATH_SUFFIXES include include/dal
    )

  ## Remove duplicate entries

  list (REMOVE_DUPLICATES DAL_INCLUDES)
  
  ##_____________________________________________________________________________
  ## Check for the library

  set (DAL_LIBRARIES "")
  
  find_library (DAL_DAL_LIBRARY lofardal
    HINTS ${DAL_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${lib_locations}
    PATH_SUFFIXES lib lib/dal
    )
  if (DAL_DAL_LIBRARY)
    list (APPEND DAL_LIBRARIES ${DAL_DAL_LIBRARY})
  endif (DAL_DAL_LIBRARY)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found

  if (DAL_INCLUDES AND DAL_LIBRARIES)
    set (DAL_FOUND TRUE)

    ## __________________________________________________________________________
    ## Find library version
    file (STRINGS "${DAL_INCLUDES}/dal_config.h" DAL_H REGEX "^#define DAL_VERSION \"[^\"]*\"$")
    string (REGEX REPLACE "^.*DAL_VERSION \"([0-9]+).*$" "\\1" DAL_VERSION_MAJOR "${DAL_H}")
    string (REGEX REPLACE "^.*DAL_VERSION \"[0-9]+\\.([0-9]+).*$" "\\1" DAL_VERSION_MINOR  "${DAL_H}")
    string (REGEX REPLACE "^.*DAL_VERSION \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" DAL_VERSION_PATCH "${DAL_H}")
    set (DAL_VERSION "${DAL_VERSION_MAJOR}.${DAL_VERSION_MINOR}.${DAL_VERSION_PATCH}")

  else (DAL_INCLUDES AND DAL_LIBRARIES)
    set (DAL_FOUND FALSE)
    if (NOT DAL_FIND_QUIETLY)
      if (NOT DAL_INCLUDES)
	message (STATUS "Unable to find DAL header files!")
      endif (NOT DAL_INCLUDES)
      if (NOT DAL_LIBRARIES)
	message (STATUS "Unable to find DAL library files!")
      endif (NOT DAL_LIBRARIES)
    endif (NOT DAL_FIND_QUIETLY)
  endif (DAL_INCLUDES AND DAL_LIBRARIES)
  
  if (DAL_FOUND)
    if (NOT DAL_FIND_QUIETLY)
      message (STATUS "Found components for DAL")
      message (STATUS "DAL_ROOT_DIR  = ${DAL_ROOT_DIR}")
      message (STATUS "DAL_INCLUDES  = ${DAL_INCLUDES}")
      message (STATUS "DAL_LIBRARIES = ${DAL_LIBRARIES}")
      message (STATUS "DAL_VERSION   = ${DAL_VERSION}")
    endif (NOT DAL_FIND_QUIETLY)
  else (DAL_FOUND)
    if (DAL_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find DAL!")
    endif (DAL_FIND_REQUIRED)
  endif (DAL_FOUND)

  ## Compatibility settings
  
  if (DAL_FOUND)
    set (HAVE_DAL TRUE)
  else (DAL_FOUND)
    set (HAVE_DAL FALSE)
  endif (DAL_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    DAL_INCLUDES
    DAL_LIBRARIES
    )
  
endif (NOT DAL_FOUND)
