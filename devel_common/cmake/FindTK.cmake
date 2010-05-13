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

# - Check for the presence of TK
#
# The following variables are set when TK is found:
#  HAVE_TK       = Set to true, if all components of TK have been found.
#  TK_INCLUDES   = Include path for the header files of TK
#  TK_LIBRARIES  = Link these to use TK
#  TK_LFGLAS     = Linker flags (optional)

if (NOT FIND_TK_CMAKE)

  set (FIND_TK_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Standard locations where to look for required components
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (TK_INCLUDES tk.h
    PATHS ${include_locations}
    PATH_SUFFIXES tk tk8.4 tk8.3 tcl tcl8.4 tcl8.3
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (TK_LIBRARIES tk tk8.4 tk8.3
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (TK_INCLUDES AND TK_LIBRARIES)
    set (HAVE_TK TRUE)
  else (TK_INCLUDES AND TK_LIBRARIES)
    set (HAVE_TK FALSE)
    if (NOT TK_FIND_QUIETLY)
      if (NOT TK_INCLUDES)
	message (STATUS "Unable to find TK header files!")
      endif (NOT TK_INCLUDES)
      if (NOT TK_LIBRARIES)
	message (STATUS "Unable to find TK library files!")
      endif (NOT TK_LIBRARIES)
    endif (NOT TK_FIND_QUIETLY)
  endif (TK_INCLUDES AND TK_LIBRARIES)
  
  if (HAVE_TK)
    if (NOT TK_FIND_QUIETLY)
      message (STATUS "Found components for TK")
      message (STATUS "TK_INCLUDES  = ${TK_INCLUDES}")
      message (STATUS "TK_LIBRARIES = ${TK_LIBRARIES}")
    endif (NOT TK_FIND_QUIETLY)
  else (HAVE_TK)
    if (TK_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find TK!")
    endif (TK_FIND_REQUIRED)
  endif (HAVE_TK)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    TK_INCLUDES
    TK_LIBRARIES
    )
  
endif (NOT FIND_TK_CMAKE)
