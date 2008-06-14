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

# - Check for the presence of <PACKAGE>
#
# The following variables are set when <PACKAGE> is found:
#  HAVE_<PACKAGE>       = Set to true, if all components of <PACKAGE>
#                          have been found.
#  <PACKAGE>_INCLUDES   = Include path for the header files of <PACKAGE>
#  <PACKAGE>_LIBRARIES  = Link these to use <PACKAGE>
#  <PACKAGE>_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (<PACKAGE>_INCLUDES <header file(s)>
  PATHS ${include_locations}
  PATH_SUFFIXES <optional path extension>
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (<PACKAGE>_LIBRARIES <package name>
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)
  set (HAVE_<PACKAGE> TRUE)
else (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)
  set (HAVE_<PACKAGE> FALSE)
  if (NOT <PACKAGE>_FIND_QUIETLY)
    if (NOT <PACKAGE>_INCLUDES)
      message (STATUS "Unable to find <PACKAGE> header files!")
    endif (NOT <PACKAGE>_INCLUDES)
    if (NOT <PACKAGE>_LIBRARIES)
      message (STATUS "Unable to find <PACKAGE> library files!")
    endif (NOT <PACKAGE>_LIBRARIES)
  endif (NOT <PACKAGE>_FIND_QUIETLY)
endif (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)

if (HAVE_<PACKAGE>)
  if (NOT <PACKAGE>_FIND_QUIETLY)
    message (STATUS "Found components for <PACKAGE>")
    message (STATUS "<PACKAGE>_INCLUDES  = ${<PACKAGE>_INCLUDES}")
    message (STATUS "<PACKAGE>_LIBRARIES = ${<PACKAGE>_LIBRARIES}")
  endif (NOT <PACKAGE>_FIND_QUIETLY)
else (HAVE_<PACKAGE>)
  if (<PACKAGE>_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find <PACKAGE>!")
  endif (<PACKAGE>_FIND_REQUIRED)
endif (HAVE_<PACKAGE>)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  <PACKAGE>_INCLUDES
  <PACKAGE>_LIBRARIES
  )
