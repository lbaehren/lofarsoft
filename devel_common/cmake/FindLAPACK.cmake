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

## -----------------------------------------------------------------------------
## Check for the presence of LAPACK
##
## The following variables are set when LAPACK is found:
##  HAVE_LAPACK      - Set to true, if all components of LAPACK have been found.
##  LAPACK_LIBRARIES - Link these to use LAPACK
##  LAPACK_INCLUDES  - Location of the LAPACK header files

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (HAVE_CLAPACK_H clapack.h
  PATHS
  ${include_locations}
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include
  /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers
  /opt/aips++/local/include/atlas
)

## -----------------------------------------------------------------------------
## Check for the library files (-llapack -lblas -lcblas -latlas)

set (LAPACK_LIBRARIES "")

## Find liblapack

find_library (HAVE_LIBLAPACK lapack
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lapack
  )

if (HAVE_LIBLAPACK)
  list (APPEND LAPACK_LIBRARIES ${HAVE_LIBLAPACK})
endif (HAVE_LIBLAPACK)

## Find libcblas

find_library (HAVE_LIBCBLAS cblas
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lapack
  )

if (HAVE_LIBCBLAS)
  list (APPEND LAPACK_LIBRARIES ${HAVE_LIBCBLAS})
endif (HAVE_LIBCBLAS)

## Find libblas

find_library (HAVE_LIBBLAS blas
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lapack
  )

if (HAVE_LIBBLAS)
  list (APPEND LAPACK_LIBRARIES ${HAVE_LIBBLAS})
endif (HAVE_LIBBLAS)

## Find libatlas

find_library (HAVE_LIBATLAS atlas
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lapack
  )

if (HAVE_LIBATLAS)
  list (APPEND LAPACK_LIBRARIES ${HAVE_LIBATLAS})
endif (HAVE_LIBATLAS)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (LAPACK_LIBRARIES)
  set (HAVE_LAPACK TRUE)
else (LAPACK_LIBRARIES)
  set (HAVE_LAPACK FALSE)
  if (NOT LAPACK_FIND_QUIETLY)
    message (STATUS "Unable to find LAPACK library files!")
  endif (NOT LAPACK_FIND_QUIETLY)
endif (LAPACK_LIBRARIES)

IF (HAVE_LAPACK)
  IF (NOT LAPACK_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LAPACK.")
    MESSAGE (STATUS "HAVE_LIBLAPACK = ${HAVE_LIBLAPACK}")
    MESSAGE (STATUS "HAVE_LIBCBLAS  = ${HAVE_LIBCBLAS}")
    MESSAGE (STATUS "HAVE_LIBBLAS   = ${HAVE_LIBBLAS}")
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ELSE (HAVE_LAPACK)
  IF (LAPACK_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LAPACK!")
  ENDIF (LAPACK_FIND_REQUIRED)
ENDIF (HAVE_LAPACK)

## -----------------------------------------------------------------------------
## Mark as advanced...

MARK_AS_ADVANCED (
  HAVE_LIBATLAS
  HAVE_LIBBLAS
  HAVE_LIBCBLAS
  HAVE_LIBLAPACK
  HAVE_CLAPACK_H
  )
