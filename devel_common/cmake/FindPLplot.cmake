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

# - Check for the presence of PLplot
#
# The following variables are set when PLPLOT is found:
#  HAVE_PLPLOT       = Set to true, if all components of PLplot have been found.
#  PLPLOT_INCLUDES   = Include path for the header files of PLplot
#  PLPLOT_LIBRARIES  = Link these to use PLplot
#  PLPLOT_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PLPLOT_INCLUDES plplot/plplot.h plplot/plplotcanvas.h
  PATHS ${include_locations}
  PATH_SUFFIXES plplot
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

## [1] libplplotd

find_library (PLPLOT_plplotd plplotd
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (PLPLOT_plplotd)
  list (APPEND PLPLOT_LIBRARIES ${PLPLOT_plplotd})
endif (PLPLOT_plplotd)

## [2] libplplotcxxd

find_library (PLPLOT_plplotcxxd plplotcxxd
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (PLPLOT_plplotcxxd)
  list (APPEND PLPLOT_LIBRARIES ${PLPLOT_plplotcxxd})
endif (PLPLOT_plplotcxxd)

## [3] libcsirocsa

find_library (PLPLOT_csirocsa csirocsa
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (PLPLOT_csirocsa)
  list (APPEND PLPLOT_LIBRARIES ${PLPLOT_csirocsa})
endif (PLPLOT_csirocsa)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)
  set (HAVE_PLPLOT TRUE)
else (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)
  set (HAVE_PLPLOT FALSE)
  if (NOT PLPLOT_FIND_QUIETLY)
    if (NOT PLPLOT_INCLUDES)
      message (STATUS "Unable to find PLPLOT header files!")
    endif (NOT PLPLOT_INCLUDES)
    if (NOT PLPLOT_LIBRARIES)
      message (STATUS "Unable to find PLPLOT library files!")
    endif (NOT PLPLOT_LIBRARIES)
  endif (NOT PLPLOT_FIND_QUIETLY)
endif (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)

if (HAVE_PLPLOT)
  if (NOT PLPLOT_FIND_QUIETLY)
    message (STATUS "Found components for PLPLOT")
    message (STATUS "PLPLOT_INCLUDES  = ${PLPLOT_INCLUDES}")
    message (STATUS "PLPLOT_LIBRARIES = ${PLPLOT_LIBRARIES}")
  endif (NOT PLPLOT_FIND_QUIETLY)
else (HAVE_PLPLOT)
  if (PLPLOT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PLPLOT!")
  endif (PLPLOT_FIND_REQUIRED)
endif (HAVE_PLPLOT)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PLPLOT_INCLUDES
  PLPLOT_LIBRARIES
  )
