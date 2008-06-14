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

# - Check for the presence of the CPGPLOT library
#
# Defines the following variables:
#  CPGPLOT_LIBRARY     -- Path to cpgplot.{a,so}
#  CPGPLOT_INCLUDE_DIR -- Directory containing the CPGPLOT header files

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (CPGPLOT_LIBRARY
  NAMES cpgplot
  PATHS ${lib_locations}
  PATH_SUFFIXES pgplot
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CPGPLOT_INCLUDE_DIR
	cpgplot.h
	PATHS ${include_locations}
	PATH_SUFFIXES include/pgplot lib/pgplot
)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)
	SET (CPGPLOT_FOUND TRUE)
ELSE (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		IF (NOT CPGPLOT_INCLUDE_DIR)
			MESSAGE (STATUS "Unable to find CPGPLOT header files!")
		ENDIF (NOT CPGPLOT_INCLUDE_DIR)
		IF (NOT CPGPLOT_LIBRARY)
			MESSAGE (STATUS "Unable to find CPGPLOT library files!")
		ENDIF (NOT CPGPLOT_LIBRARY)
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ENDIF (CPGPLOT_INCLUDE_DIR AND CPGPLOT_LIBRARY)


IF (CPGPLOT_FOUND)
	IF (NOT CPGPLOT_FIND_QUIETLY)
		MESSAGE (STATUS "Found components for CPGPLOT")
		MESSAGE (STATUS "CPGPLOT_LIBRARY     = ${CPGPLOT_LIBRARY}")
		MESSAGE (STATUS "CPGPLOT_INCLUDE_DIR = ${CPGPLOT_INCLUDE_DIR}")
	ENDIF (NOT CPGPLOT_FIND_QUIETLY)
ELSE (CPGPLOT_FOUND)
	IF (CPGPLOT_FIND_REQUIRED)
		MESSAGE (FATAL_ERROR "Could not find CPGPLOT!")
	ENDIF (CPGPLOT_FIND_REQUIRED)
ENDIF (CPGPLOT_FOUND)
