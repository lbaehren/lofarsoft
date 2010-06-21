# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
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

# - Check for the presence of CRTOOLS
#
# The following variables are set when CRTOOLS is found:
#  HAVE_CRTOOLS       = Set to true, if all components of CRTOOLS have been found.
#  CRTOOLS_INCLUDES   = Include path for the header files of CRTOOLS
#  CRTOOLS_LIBRARIES  = Link these to use CRTOOLS
#  CRTOOLS_LFLAGS     = Linker flags (optional)

if (NOT FIND_CRTOOLS_CMAKE)
  
  set (FIND_CRTOOLS_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (CRTOOLS_INCLUDES Analysis/CompletePipeline.h IO/LOFAR_TBB.h
    PATHS ${include_locations}
    PATH_SUFFIXES cr crtools
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (CRTOOLS_CR_LIBRARY cr
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )

  if (CRTOOLS_CR_LIBRARY)
    list (APPEND CRTOOLS_LIBRARIES ${CRTOOLS_CR_LIBRARY})
  endif (CRTOOLS_CR_LIBRARY)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (CRTOOLS_INCLUDES AND CRTOOLS_LIBRARIES)
    set (HAVE_CRTOOLS TRUE)
  else (CRTOOLS_INCLUDES AND CRTOOLS_LIBRARIES)
    set (HAVE_CRTOOLS FALSE)
    if (NOT CRTOOLS_FIND_QUIETLY)
      if (NOT CRTOOLS_INCLUDES)
	message (STATUS "Unable to find CRTOOLS header files!")
      endif (NOT CRTOOLS_INCLUDES)
      if (NOT CRTOOLS_LIBRARIES)
	message (STATUS "Unable to find CRTOOLS library files!")
      endif (NOT CRTOOLS_LIBRARIES)
    endif (NOT CRTOOLS_FIND_QUIETLY)
  endif (CRTOOLS_INCLUDES AND CRTOOLS_LIBRARIES)
  
  if (HAVE_CRTOOLS)
    if (NOT CRTOOLS_FIND_QUIETLY)
      message (STATUS "Found components for CRTOOLS")
      message (STATUS "CRTOOLS_INCLUDES  = ${CRTOOLS_INCLUDES}")
      message (STATUS "CRTOOLS_LIBRARIES = ${CRTOOLS_LIBRARIES}")
    endif (NOT CRTOOLS_FIND_QUIETLY)
  else (HAVE_CRTOOLS)
    if (CRTOOLS_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find CRTOOLS!")
    endif (CRTOOLS_FIND_REQUIRED)
  endif (HAVE_CRTOOLS)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    CRTOOLS_INCLUDES
    CRTOOLS_LIBRARIES
    )
  
endif (NOT FIND_CRTOOLS_CMAKE)
