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

# - Check for the presence of Glish
#
# The following variables are set when Blitz++ is found:
#  HAVE_GLISH      = Set to true, if all components of Glish have been found.
#  GLISH_INCLUDES  = Include path for the header files of Glish
#  GLISH_LIBRARIES = Link these to use Glish

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

if (NOT casa_locations)
  set (casa_locations
    ## most Linux systems
    /aips++
    /casa
    /opt/aips++/stable
    /opt/aips++/current
    /opt/aips++
    /opt/casa
    /opt/casa/stable
    /opt/casa/current
    ## Mac OS X (Fink)
    /sw/share/aips++
    /sw/share/casa
    ## LOFAR development
    /app/aips++/Stable
    )
endif (NOT casa_locations)

## -----------------------------------------------------------------------------
## Check for the header files

## [1] Glish/glish.h

find_path (GLISH_INCLUDES Glish/glish.h
  PATHS ${casa_locations}
  PATH_SUFFIXES code/aips/glish/include code/aips/glish/glish/include
  )

## [2] sos/sos.h

find_path (sos_sos_h sos/sos.h
  PATHS ${casa_locations}
  PATH_SUFFIXES code/aips/glish/sos/include
  )

if (sos_sos_h)
  list (APPEND GLISH_INCLUDES ${sos_sos_h})
endif (sos_sos_h)

## [2] Npd/npd.h

find_path (npd_npd_h Npd/npd.h npd/npd.h
  PATHS ${casa_locations}
  PATH_SUFFIXES code/aips/glish/npd/include
  )

if (npd_npd_h)
  list (APPEND GLISH_INCLUDES ${npd_npd_h})
endif (npd_npd_h)

## -----------------------------------------------------------------------------
## Check for the library

set (GLISH_LIBRARIES "")

## [1] libglish

find_library (libglish glish
  PATHS ${casa_locations}
  PATH_SUFFIXES linux/lib linux_gnu/lib
  )

if (libglish) 
  list (APPEND GLISH_LIBRARIES ${libglish})
endif (libglish)

## [2] libsos

find_library (libsos sos
  PATHS ${casa_locations}
  PATH_SUFFIXES linux/lib linux_gnu/lib
  )

if (libsos)
  list (APPEND GLISH_LIBRARIES ${libsos})
endif (libsos)

## [3] libnpd

find_library (libnpd npd
  PATHS ${casa_locations}
  PATH_SUFFIXES linux/lib linux_gnu/lib
  )

if (libnpd)
  list (APPEND GLISH_LIBRARIES ${libnpd})
endif (libnpd)

## [4] libregx

find_library (libregx regx
  PATHS ${casa_locations}
  PATH_SUFFIXES ${lib_locations}
  )

if (libregx)
  list (APPEND GLISH_LIBRARIES ${libregx})
endif (libregx)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

SET (HAVE_GLISH FALSE)

IF (GLISH_INCLUDES AND GLISH_LIBRARIES)
  SET (HAVE_GLISH TRUE)
ELSE (GLISH_INCLUDES AND GLISH_LIBRARIES)
  SET (HAVE_GLISH FALSE)
  IF (NOT GLISH_FIND_QUIETLY)
    IF (NOT GLISH_INCLUDES)
      MESSAGE (STATUS "Unable to find Glish header files!")
    ENDIF (NOT GLISH_INCLUDES)
    IF (NOT GLISH_LIBRARIES)
      MESSAGE (STATUS "Unable to find Glish library files!")
    ENDIF (NOT GLISH_LIBRARIES)
  ENDIF (NOT GLISH_FIND_QUIETLY)
ENDIF (GLISH_INCLUDES AND GLISH_LIBRARIES)

IF (HAVE_GLISH)
  IF (NOT GLISH_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for Glish")
    MESSAGE (STATUS "GLISH_INCLUDES  = ${GLISH_INCLUDES}")
    MESSAGE (STATUS "GLISH_LIBRARIES = ${GLISH_LIBRARIES}")
  ENDIF (NOT GLISH_FIND_QUIETLY)
ELSE (HAVE_GLISH)
  IF (GLISH_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find Glish!")
  ENDIF (GLISH_FIND_REQUIRED)
ENDIF (HAVE_GLISH)

## -----------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  HAVE_GLISH
  GLISH_INCLUDES
  GLISH_LIBRARIES
  libglish
  libsos
  libnpd
  )
