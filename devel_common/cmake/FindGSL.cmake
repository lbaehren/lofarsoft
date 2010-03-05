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

# - Check for the presence of GSL
#
# The following variables are set when GSL is found:
#  HAVE_GSL       = Set to true, if all components of GSL have been found.
#  GSL_INCLUDES   = Include path for the header files of GSL
#  GSL_LIBRARIES  = Link these to use GSL

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GSL_INCLUDES gsl_version.h
  PATHS ${include_locations} /data/sys/opt/gsl/gsl-1.13/include
  PATH_SUFFIXES gsl
  NO_DEFAULT_PATH
  )

## adjust the include path to strip of the "gsl"
string (REGEX REPLACE include/gsl include GSL_INCLUDES ${GSL_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

set (GSL_LIBRARIES "")

## [1] libgsl

find_library (GSL_GSL_LIBRARY gsl
  PATHS ${lib_locations} /data/sys/opt/gsl/gsl-1.13/lib
  NO_DEFAULT_PATH
  )

if (GSL_GSL_LIBRARY)
  list (APPEND GSL_LIBRARIES ${GSL_GSL_LIBRARY})
endif (GSL_GSL_LIBRARY)

## [2] libgslcblas

find_library (GSL_CBLAS_LIBRARY gslcblas
  PATHS ${lib_locations} /data/sys/opt/gsl/gsl-1.13/lib
  NO_DEFAULT_PATH
  )

if (GSL_CBLAS_LIBRARY)
  list (APPEND GSL_LIBRARIES ${GSL_CBLAS_LIBRARY})
endif (GSL_CBLAS_LIBRARY)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GSL_INCLUDES AND GSL_LIBRARIES)
  set (HAVE_GSL TRUE)
else (GSL_INCLUDES AND GSL_LIBRARIES)
  set (HAVE_GSL FALSE)
  if (NOT GSL_FIND_QUIETLY)
    if (NOT GSL_INCLUDES)
      message (STATUS "Unable to find GSL header files!")
    endif (NOT GSL_INCLUDES)
    if (NOT GSL_LIBRARIES)
      message (STATUS "Unable to find GSL library files!")
    endif (NOT GSL_LIBRARIES)
  endif (NOT GSL_FIND_QUIETLY)
endif (GSL_INCLUDES AND GSL_LIBRARIES)

if (HAVE_GSL)
  if (NOT GSL_FIND_QUIETLY)
    message (STATUS "Found components for GSL")
    message (STATUS "GSL_INCLUDES  = ${GSL_INCLUDES}")
    message (STATUS "GSL_LIBRARIES = ${GSL_LIBRARIES}")
  endif (NOT GSL_FIND_QUIETLY)
else (HAVE_GSL)
  if (GSL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GSL!")
  endif (GSL_FIND_REQUIRED)
endif (HAVE_GSL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GSL_INCLUDES
  GSL_LIBRARIES
  )
