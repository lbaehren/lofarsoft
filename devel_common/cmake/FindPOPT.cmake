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

# - Check for the presence of POPT
#
# The following variables are set when POPT is found:
#  HAVE_POPT       = Set to true, if all components of POPT have been found.
#  POPT_INCLUDES   = Include path for the header files of POPT
#  POPT_LIBRARIES  = Link these to use POPT

if (NOT FIND_POPT_CMAKE)
  
  set (FIND_POPT_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Standard locations where to look for required components
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  FIND_PATH (POPT_INCLUDES popt.h
    PATHS ${include_locations}
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  FIND_LIBRARY (POPT_LIBRARIES popt
    PATHS ${lib_locations}
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  IF (POPT_INCLUDES AND POPT_LIBRARIES)
    SET (HAVE_POPT TRUE)
  ELSE (POPT_INCLUDES AND POPT_LIBRARIES)
    SET (HAVE_POPT FALSE)
    IF (NOT POPT_FIND_QUIETLY)
      IF (NOT POPT_INCLUDES)
	MESSAGE (STATUS "Unable to find POPT header files!")
      ENDIF (NOT POPT_INCLUDES)
      IF (NOT POPT_LIBRARIES)
	MESSAGE (STATUS "Unable to find POPT library files!")
      ENDIF (NOT POPT_LIBRARIES)
    ENDIF (NOT POPT_FIND_QUIETLY)
  ENDIF (POPT_INCLUDES AND POPT_LIBRARIES)
  
  IF (HAVE_POPT)
    IF (NOT POPT_FIND_QUIETLY)
      MESSAGE (STATUS "Found components for POPT")
      MESSAGE (STATUS "POPT_INCLUDES   = ${POPT_INCLUDES}")
      MESSAGE (STATUS "POPT_LIBRARIES  = ${POPT_LIBRARIES}")
    ENDIF (NOT POPT_FIND_QUIETLY)
  ELSE (HAVE_POPT)
    IF (POPT_FIND_REQUIRED)
      MESSAGE (FATAL_ERROR "Could not find POPT!")
    ENDIF (POPT_FIND_REQUIRED)
  ENDIF (HAVE_POPT)
  
endif (NOT FIND_POPT_CMAKE)
