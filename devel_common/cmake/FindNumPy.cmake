# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
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

# - Check for the presence of NUMPY
#
# The following variables are set when NUMPY is found:
#  HAVE_NUMPY       = Set to true, if all components of NUMPY
#                          have been found.
#  NUMPY_INCLUDES           = Include path for the header files of NUMPY
#  NUMPY_MULTIARRAY_LIBRARY = Path to the multiarray shared library
#  NUMPY_SCALARMATH_LIBRARY = Path to the scalarmath shared library
#  NUMPY_LIBRARIES          = Link these to use NUMPY
#  F2PY_EXECUTABLE          = The f2py executable
#  NUMPY_LFLAGS             = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NUMPY_INCLUDES numpy/__multiarray_api.h numpy/multiarray_api.txt
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python2.6/site-packages/numpy/core/include
  python2.5/site-packages/numpy/core/include
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

set (CMAKE_FIND_LIBRARY_PREFIXES "" CACHE STRING
  "Library prefixes"
  FORCE
  )

find_library (NUMPY_MULTIARRAY_LIBRARY multiarray
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python2.6/site-packages/numpy/core
  python2.5/site-packages/numpy/core
  python2.4/site-packages/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_MULTIARRAY_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_MULTIARRAY_LIBRARY})
endif (NUMPY_MULTIARRAY_LIBRARY)

find_library (NUMPY_SCALARMATH_LIBRARY scalarmath
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python2.6/site-packages/numpy/core
  python2.5/site-packages/numpy/core
  python2.4/site-packages/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_SCALARMATH_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_SCALARMATH_LIBRARY})
endif (NUMPY_SCALARMATH_LIBRARY)

## -----------------------------------------------------------------------------
## Check for executables

find_program (F2PY_EXECUTABLE f2py f2py2.6 f2py2.5 f2py2.4
  PATHS ${bin_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (NUMPY_INCLUDES AND NUMPY_LIBRARIES)
  set (HAVE_NUMPY TRUE)
else (NUMPY_INCLUDES AND NUMPY_LIBRARIES)
  set (HAVE_NUMPY FALSE)
  if (NOT NUMPY_FIND_QUIETLY)
    if (NOT NUMPY_INCLUDES)
      message (STATUS "Unable to find NUMPY header files!")
    endif (NOT NUMPY_INCLUDES)
    if (NOT NUMPY_LIBRARIES)
      message (STATUS "Unable to find NUMPY library files!")
    endif (NOT NUMPY_LIBRARIES)
  endif (NOT NUMPY_FIND_QUIETLY)
endif (NUMPY_INCLUDES AND NUMPY_LIBRARIES)

if (HAVE_NUMPY)
  if (NOT NUMPY_FIND_QUIETLY)
    message (STATUS "Found components for NUMPY")
    message (STATUS "NUMPY_INCLUDES  = ${NUMPY_INCLUDES}")
    message (STATUS "NUMPY_LIBRARIES = ${NUMPY_LIBRARIES}")
  endif (NOT NUMPY_FIND_QUIETLY)
else (HAVE_NUMPY)
  if (NUMPY_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NUMPY!")
  endif (NUMPY_FIND_REQUIRED)
endif (HAVE_NUMPY)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NUMPY_INCLUDES
  NUMPY_LIBRARIES
  NUMPY_MULTIARRAY_LIBRARY
  NUMPY_SCALARMATH_LIBRARY
  F2PY_EXECUTABLE
  )
