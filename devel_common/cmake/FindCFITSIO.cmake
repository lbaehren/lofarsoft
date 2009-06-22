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

# - Check for the presence of the CFITSIO library
#
#  HAVE_CFITSIO      = Do we have CFITSIO?
#  CFITSIO_LIBRARIES = Set of libraries required for linking against CFITSIO
#  CFITSIO_INCLUDES  = Directory where to find fitsio.h

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

set (CFITSIO_INCLUDES "")

FIND_PATH (HAVE_FITSIO_H fitsio.h
  PATHS ${include_locations}
  PATH_SUFFIXES cfitsio
  NO_DEFAULT_PATH
  )

if (HAVE_FITSIO_H)
  list (APPEND CFITSIO_INCLUDES ${HAVE_FITSIO_H})
endif (HAVE_FITSIO_H)

## -----------------------------------------------------------------------------
## Check for the parts of the library

## [1] core library

FIND_LIBRARY (CFITSIO_libcfitsio
  NAMES cfitsio
  PATHS ${lib_locations}
  PATH_SUFFIXES cfitsio
  NO_DEFAULT_PATH
)

if (CFITSIO_libcfitsio)
  set (CFITSIO_LIBRARIES ${CFITSIO_libcfitsio})
endif (CFITSIO_libcfitsio)

## [2] math library

FIND_LIBRARY (CFITSIO_libm
  NAMES m
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
)

## [3] file access

FIND_LIBRARY (CFITSIO_libnsl
  NAMES nsl
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (CFITSIO_libnsl)
  list (APPEND CFITSIO_LIBRARIES ${CFITSIO_libnsl})
else (CFITSIO_libnsl)
  message (STATUS "Unable to find libnsl.")
endif (CFITSIO_libnsl)

## -----------------------------------------------------------------------------
## Check libcfitsio for required symbols

include (CheckLibraryExists)

if (CFITSIO_libcfitsio)
    foreach (libsymbol file_open ftp_open file_openfile ffopen ffopentest ffreopen)    
    check_library_exists (
      ${CFITSIO_libcfitsio}
      ${libsymbol}
      ""
      libcfitsio_has_${libsymbol}
      )
  endforeach (libsymbol)
endif (CFITSIO_libcfitsio)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
  SET (HAVE_CFITSIO TRUE)
ELSE (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
  IF (NOT CFITSIO_FIND_QUIETLY)
    IF (NOT CFITSIO_INCLUDES)
      MESSAGE (STATUS "Unable to find CFITSIO header files!")
    ENDIF (NOT CFITSIO_INCLUDES)
    IF (NOT CFITSIO_LIBRARIES)
      MESSAGE (STATUS "Unable to find CFITSIO library files!")
    ENDIF (NOT CFITSIO_LIBRARIES)
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ENDIF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)

IF (HAVE_CFITSIO)
  IF (NOT CFITSIO_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for CFITSIO")
    MESSAGE (STATUS "CFITSIO_LIBRARIES = ${CFITSIO_LIBRARIES}")
    MESSAGE (STATUS "CFITSIO_INCLUDES  = ${CFITSIO_INCLUDES}")
  ENDIF (NOT CFITSIO_FIND_QUIETLY)
ELSE (HAVE_CFITSIO)
  IF (CFITSIO_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
  ENDIF (CFITSIO_FIND_REQUIRED)
ENDIF (HAVE_CFITSIO)

## ------------------------------------------------------------------------------
## Mark as advanced ...

MARK_AS_ADVANCED (
  CFITSIO_libcfitsio
  CFITSIO_libm
  CFITSIO_INCLUDES
  CFITSIO_LIBRARIES
  HAVE_FITSIO_H
  )

if (CFITSIO_libnsl)
  mark_as_advanced (
    CFITSIO_libnsl
    )
endif (CFITSIO_libnsl)
