# +-----------------------------------------------------------------------------+
# | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                               $ |
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

# - Check for the presence of STARTOOLS
#
# The following variables are set when STARTOOLS is found:
#  HAVE_STARTOOLS       = Set to true, if all components of STARTOOLS
#                          have been found.
#  STARTOOLS_INCLUDES   = Include path for the header files of STARTOOLS
#  STARTOOLS_LIBRARIES  = Link these to use STARTOOLS
#  STARTOOLS_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (STARTOOLS_INCLUDES startools/util.hh startools/trigger.hh
  PATHS ${include_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

set (STARTOOLS_LIBRARIES "")

find_library (libstarutil starutil
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (libstarutil)
  list (APPEND STARTOOLS_LIBRARIES ${libstarutil})
endif (libstarutil)

find_library (librecradio recradio
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (librecradio)
  list (APPEND STARTOOLS_LIBRARIES ${librecradio})
endif (librecradio)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)
  set (HAVE_STARTOOLS TRUE)
else (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)
  set (HAVE_STARTOOLS FALSE)
  if (NOT STARTOOLS_FIND_QUIETLY)
    if (NOT STARTOOLS_INCLUDES)
      message (STATUS "Unable to find STARTOOLS header files!")
    endif (NOT STARTOOLS_INCLUDES)
    if (NOT STARTOOLS_LIBRARIES)
      message (STATUS "Unable to find STARTOOLS library files!")
    endif (NOT STARTOOLS_LIBRARIES)
  endif (NOT STARTOOLS_FIND_QUIETLY)
endif (STARTOOLS_INCLUDES AND STARTOOLS_LIBRARIES)

if (HAVE_STARTOOLS)
  if (NOT STARTOOLS_FIND_QUIETLY)
    message (STATUS "Found components for STARTOOLS")
    message (STATUS "STARTOOLS_INCLUDES  = ${STARTOOLS_INCLUDES}")
    message (STATUS "STARTOOLS_LIBRARIES = ${STARTOOLS_LIBRARIES}")
  endif (NOT STARTOOLS_FIND_QUIETLY)
else (HAVE_STARTOOLS)
  if (STARTOOLS_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find STARTOOLS!")
  endif (STARTOOLS_FIND_REQUIRED)
endif (HAVE_STARTOOLS)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  STARTOOLS_INCLUDES
  STARTOOLS_LIBRARIES
  )
