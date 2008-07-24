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

# - Check for the presence of NTLM
#
# The following variables are set when NTLM is found:
#  HAVE_NTLM       = Set to true, if all components of NTLM have been found.
#  NTLM_INCLUDES   = Include path for the header files of NTLM
#  NTLM_LIBRARIES  = Link these to use NTLM
#  NTLM_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NTLM_INCLUDES ntlm.h
  PATHS ${include_locations}
  PATH_SUFFIXES ntlm sasl2 sasl2/include
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (NTLM_LIBRARIES ntlm
  PATHS ${lib_locations}
  PATH_SUFFIXES sasl2
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (NTLM_INCLUDES AND NTLM_LIBRARIES)
  set (HAVE_NTLM TRUE)
else (NTLM_INCLUDES AND NTLM_LIBRARIES)
  set (HAVE_NTLM FALSE)
  if (NOT NTLM_FIND_QUIETLY)
    if (NOT NTLM_INCLUDES)
      message (STATUS "Unable to find NTLM header files!")
    endif (NOT NTLM_INCLUDES)
    if (NOT NTLM_LIBRARIES)
      message (STATUS "Unable to find NTLM library files!")
    endif (NOT NTLM_LIBRARIES)
  endif (NOT NTLM_FIND_QUIETLY)
endif (NTLM_INCLUDES AND NTLM_LIBRARIES)

if (HAVE_NTLM)
  if (NOT NTLM_FIND_QUIETLY)
    message (STATUS "Found components for NTLM")
    message (STATUS "NTLM_INCLUDES  = ${NTLM_INCLUDES}")
    message (STATUS "NTLM_LIBRARIES = ${NTLM_LIBRARIES}")
  endif (NOT NTLM_FIND_QUIETLY)
else (HAVE_NTLM)
  if (NTLM_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NTLM!")
  endif (NTLM_FIND_REQUIRED)
endif (HAVE_NTLM)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NTLM_INCLUDES
  NTLM_LIBRARIES
  )
