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

# - Check for the presence of SZIP
#
# The following variables are set when SZIP is found:
#  HAVE_SZIP       = Set to true, if all components of SZIP have been found.
#  SZIP_INCLUDES   = Include path for the header files of SZIP
#  SZIP_LIBRARIES  = Link these to use SZIP
#  SZIP_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (SZIP_INCLUDES szlib.h
  PATHS ${include_locations}
  PATH_SUFFIXES szip
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

set (SZIP_LIBRARIES "")

## libszip

find_library (SZIP_SZIP_LIBRARY szip
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (SZIP_SZIP_LIBRARY)
  list (APPEND SZIP_LIBRARIES ${SZIP_SZIP_LIBRARY})
endif (SZIP_SZIP_LIBRARY)

## libsz

find_library (SZIP_SZ_LIBRARY sz
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (SZIP_SZ_LIBRARY)
  list (APPEND SZIP_LIBRARIES ${SZIP_SZ_LIBRARY})
endif (SZIP_SZ_LIBRARY)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (SZIP_INCLUDES AND SZIP_LIBRARIES)
  set (HAVE_SZIP TRUE)
else (SZIP_INCLUDES AND SZIP_LIBRARIES)
  set (HAVE_SZIP FALSE)
  if (NOT SZIP_FIND_QUIETLY)
    if (NOT SZIP_INCLUDES)
      message (STATUS "Unable to find SZIP header files!")
    endif (NOT SZIP_INCLUDES)
    if (NOT SZIP_LIBRARIES)
      message (STATUS "Unable to find SZIP library files!")
    endif (NOT SZIP_LIBRARIES)
  endif (NOT SZIP_FIND_QUIETLY)
endif (SZIP_INCLUDES AND SZIP_LIBRARIES)

if (HAVE_SZIP)
  if (NOT SZIP_FIND_QUIETLY)
    message (STATUS "Found components for SZIP")
    message (STATUS "SZIP_INCLUDES  = ${SZIP_INCLUDES}")
    message (STATUS "SZIP_LIBRARIES = ${SZIP_LIBRARIES}")
  endif (NOT SZIP_FIND_QUIETLY)
else (HAVE_SZIP)
  if (SZIP_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find SZIP!")
  endif (SZIP_FIND_REQUIRED)
endif (HAVE_SZIP)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  SZIP_INCLUDES
  SZIP_LIBRARIES
  SZIP_SZIP_LIBRARY
  SZIP_SZ_LIBRARY
  )
