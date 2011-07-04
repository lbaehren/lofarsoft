##########################################################################
#  This file is part of the Transmogrify library.                        #
#  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            #
#                                                                        #
#  This library is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  (at your option) any later version.                                   #
#                                                                        # 
#  This library is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this library. If not, see <http://www.gnu.org/licenses/>.  #
##########################################################################

# - Check for the presence of TMF
#
# The following variables are set when TMF is found:
#
#   TMF_FOUND       = Set to true, if all components of TMF have been found.
#   TMF_INCLUDES   = Include path for the header files of TMF
#   TMF_LIBRARIES  = Link these to use TMF
#   TMF_VERSION_MAJOR = Major version number
#   TMF_VERSION_MINOR = Minor version number
#   TMF_VERSION_PATCH = Patch version number
#   TMF_VERSION       = TMF version string

if (NOT TMF_FOUND)

  ## ---------------------------------------------------------------------
  ## Search locations
  
  include (CMakeSettings)

  if (NOT TMF_ROOT_DIR)
    set (TMF_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT TMF_ROOT_DIR)

  ## ---------------------------------------------------------------------
  ## Check for the header files
  
  find_path (TMF_INCLUDES tmf.h tmf_constants.h tmf_config.h
    HINTS ${TMF_ROOT_DIR}
    PATHS ${include_locations} / /usr /usr/local /opt /opt/local
    PATH_SUFFIXES include include/tmf
    )
  
  ## ---------------------------------------------------------------------
  ## Check for the library
  
  find_library (TMF_LIBRARIES tmf
    HINTS ${TMF_ROOT_DIR}
    PATHS ${lib_locations} / /usr /usr/local /opt /opt/local
    PATH_SUFFIXES lib lib/tmf
    )
  
  ##______________________________________________________________________
  ## Find library version

  if (TMF_INCLUDES AND EXISTS "${TMF_INCLUDES}/tmf_config.h")
    file (STRINGS "${TMF_INCLUDES}/tmf_config.h" TMF_H REGEX "^#define TMF_VERSION \"[^\"]*\"$")

    string (REGEX REPLACE "^.*TMF_VERSION \"([0-9]+).*$" "\\1" TMF_VERSION_MAJOR "${TMF_H}")
    string (REGEX REPLACE "^.*TMF_VERSION \"[0-9]+\\.([0-9]+).*$" "\\1" TMF_VERSION_MINOR  "${TMF_H}")
    string (REGEX REPLACE "^.*TMF_VERSION \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" TMF_VERSION_PATCH "${TMF_H}")
    set (TMF_VERSION "${TMF_VERSION_MAJOR}.${TMF_VERSION_MINOR}.${TMF_VERSION_PATCH}")

  else (TMF_INCLUDES AND EXISTS "${TMF_INCLUDES}/tmf_config.h")
    message (WARNING "Cannot find tmf_config.h file to determine library version.")
  endif (TMF_INCLUDES AND EXISTS "${TMF_INCLUDES}/tmf_config.h")

  ## ---------------------------------------------------------------------
  ## Actions taken when all components have been found
  
  if (TMF_INCLUDES AND TMF_LIBRARIES)
    set (TMF_FOUND TRUE)
  else (TMF_INCLUDES AND TMF_LIBRARIES)
    set (TMF_FOUND FALSE)
    if (NOT TMF_FIND_QUIETLY)
      if (NOT TMF_INCLUDES)
        message (STATUS "Unable to find TMF header files!")
      endif (NOT TMF_INCLUDES)
      if (NOT TMF_LIBRARIES)
        message (STATUS "Unable to find TMF library files!")
      endif (NOT TMF_LIBRARIES)
    endif (NOT TMF_FIND_QUIETLY)
  endif (TMF_INCLUDES AND TMF_LIBRARIES)
  
  if (TMF_FOUND)
    if (NOT TMF_FIND_QUIETLY)
      message (STATUS "Found components for TMF")
      message (STATUS "TMF_INCLUDES  = ${TMF_INCLUDES}")
      message (STATUS "TMF_LIBRARIES = ${TMF_LIBRARIES}")
      message (STATUS "TMF_VERSION = ${TMF_VERSION}")
    endif (NOT TMF_FIND_QUIETLY)
  else (TMF_FOUND)
    if (TMF_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find TMF!")
    endif (TMF_FIND_REQUIRED)
  endif (TMF_FOUND)
  
  ## ---------------------------------------------------------------------
  ## Mark as advanced ...
  
  mark_as_advanced (
    TMF_INCLUDES
    TMF_LIBRARIES
    )

endif (NOT TMF_FOUND)

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

# - Check for the presence of TMF
#
# The following variables are set when TMF is found:
#  HAVE_TMF       = Set to true, if all components of TMF have been found.
#  TMF_INCLUDES   = Include path for the header files of TMF
#  TMF_LIBRARIES  = Link these to use TMF

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (TMF_INCLUDES tmf.h
  PATHS
  ${include_locations}
  /opt/local/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (TMF_LIBRARIES tmf
  PATHS
  ${lib_locations}
  /opt/local/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (TMF_INCLUDES AND TMF_LIBRARIES)
  SET (HAVE_TMF TRUE)
ELSE (TMF_INCLUDES AND TMF_LIBRARIES)
  SET (HAVE_TMF FALSE)
  IF (NOT TMF_FIND_QUIETLY)
    IF (NOT TMF_INCLUDES)
      MESSAGE (STATUS "Unable to find TMF header files!")
    ENDIF (NOT TMF_INCLUDES)
    IF (NOT TMF_LIBRARIES)
      MESSAGE (STATUS "Unable to find TMF library files!")
    ENDIF (NOT TMF_LIBRARIES)
  ENDIF (NOT TMF_FIND_QUIETLY)
ENDIF (TMF_INCLUDES AND TMF_LIBRARIES)

IF (HAVE_TMF)
  IF (NOT TMF_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for TMF")
    MESSAGE (STATUS "TMF_INCLUDES  = ${TMF_INCLUDES}")
    MESSAGE (STATUS "TMF_LIBRARIES = ${TMF_LIBRARIES}")
  ENDIF (NOT TMF_FIND_QUIETLY)
ELSE (HAVE_TMF)
  IF (TMF_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find TMF!")
  ENDIF (TMF_FIND_REQUIRED)
ENDIF (HAVE_TMF)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  TMF_INCLUDES
  TMF_LIBRARIES
  )

