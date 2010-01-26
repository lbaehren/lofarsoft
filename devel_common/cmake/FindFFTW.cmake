# +-----------------------------------------------------------------------------+
# | $Id::                                                                    $ |
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
# |                FFTWFFTW                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

# - Check for the presence of FFTW
#
# The following variables are set when FFTW is found:
#  HAVE_FFTW       = Set to true, if all components of FFTW have been found.
#  FFTW_INCLUDES   = Include path for the header files of FFTW
#  FFTW_LIBRARIES  = Link these to use FFTW

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (FFTW_INCLUDES fftw.h
  PATHS
  ${include_locations}
  /opt/aips++/local/include
  /var/chroot/meqtrees/usr/include
  /opt/local/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (FFTW_LIBRARIES fftw 
  PATHS
  ${lib_locations}
  /opt/aips++/local/lib
  /var/chroot/meqtrees/usr/lib
  /opt/local/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (FFTW_INCLUDES AND FFTW_LIBRARIES)
  SET (HAVE_FFTW TRUE)
ELSE (FFTW_INCLUDES AND FFTW_LIBRARIES)
  SET (HAVE_FFTW FALSE)
  IF (NOT FFTW_FIND_QUIETLY)
    IF (NOT FFTW_INCLUDES)
      MESSAGE (STATUS "Unable to find FFTW header files!")
    ENDIF (NOT FFTW_INCLUDES)
    IF (NOT FFTW_LIBRARIES)
      MESSAGE (STATUS "Unable to find FFTW library files!")
    ENDIF (NOT FFTW_LIBRARIES)
  ENDIF (NOT FFTW_FIND_QUIETLY)
ENDIF (FFTW_INCLUDES AND FFTW_LIBRARIES)

IF (HAVE_FFTW)
  IF (NOT FFTW_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for FFTW")
    MESSAGE (STATUS "FFTW_INCLUDES  = ${FFTW_INCLUDES}")
    MESSAGE (STATUS "FFTW_LIBRARIES = ${FFTW_LIBRARIES}")
  ENDIF (NOT FFTW_FIND_QUIETLY)
ELSE (HAVE_FFTW)
  IF (FFTW_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find FFTW!")
  ENDIF (FFTW_FIND_REQUIRED)
ENDIF (HAVE_FFTW)

## ------------------------------------------------------------------------------
## Mark as advanced ...

