# +-----------------------------------------------------------------------------+
# | $Id:: FindLAPACK.cmake 1643 2008-06-14 10:19:20Z baehren                  $ |
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
##  HAVE_LAPACK     - Set to true, if all components of LAPACK have been found.
##  LAPACK_LIBRARIES - Link these to use LAPACK
##  LAPACK_INCLUDES  - Location of the LAPACK header files

## -----------------------------------------------------------------------------
## Check for the header files

find_path (LAPACK_INCLUDES clapack.h
  PATHS
  ${include_locations}
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include
  /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers
)

## -----------------------------------------------------------------------------
## Check for the library files (-llapack -lblas -lcblas -latlas)

set (libs
  lapack
  cblas
  blas
  atlas
  )

set (LAPACK_LIBRARIES "")

foreach (lib ${libs})
  ## try to locate the library
  find_library (LAPACK_${lib} ${lib} ${lib}_LINUX
    PATHS
	${lib_locations}
    /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
    PATH_SUFFIXES lapack
    NO_DEFAULT_PATH
    )
  ## check if location was successful
  if (LAPACK_${lib})
    list (APPEND LAPACK_LIBRARIES ${LAPACK_${lib}})
  endif (LAPACK_${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (LAPACK_LIBRARIES)
  SET (HAVE_LAPACK TRUE)
ELSE (LAPACK_LIBRARIES)
  IF (NOT LAPACK_FIND_QUIETLY)
    IF (NOT LAPACK_LIBRARIES)
      MESSAGE (STATUS "Unable to find LAPACK library files!")
    ENDIF (NOT LAPACK_LIBRARIES)
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ENDIF (LAPACK_LIBRARIES)

IF (HAVE_LAPACK)
  IF (NOT LAPACK_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LAPACK")
    MESSAGE (STATUS "LAPACK_INCLUDES  = ${LAPACK_INCLUDES}")
    MESSAGE (STATUS "LAPACK_LIBRARIES = ${LAPACK_LIBRARIES}")
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ELSE (HAVE_LAPACK)
  IF (LAPACK_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LAPACK!")
  ENDIF (LAPACK_FIND_REQUIRED)
ENDIF (HAVE_LAPACK)

## -----------------------------------------------------------------------------
## Mark as advanced...

MARK_AS_ADVANCED (
  LAPACK_atlas
  LAPACK_blas
  LAPACK_cblas
  LAPACK_lapack
  )
