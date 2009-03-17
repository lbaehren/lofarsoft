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

# - Check for the presence of <PACKAGE>
#
# The following variables are set when PYTHON is found:
#
#  HAVE_PYTHON       = Set to true, if all components of PYTHON have been found.
#  PYTHON_EXECUTABLE = Location of the Python executable
#  PYTHON_INCLUDES   = Include path for the header files of PYTHON
#  PYTHON_LIBRARIES  = Link these to use PYTHON
#  PYTHON_LFGLAS     = Linker flags (optional)
#
# Beside the core components of Python we also include a search for optional
# packages which might be installed as well:
#
#  NUMPY_INCLUDES    = Include path for the header files of NUMPY package
#  NUM_UTIL_INCLUDES = Include path for the header files of NUM_UTIL package
#

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Is the root of the Python installation defined through environment variable?

set (HAVE_PYTHONHOME $ENV{PYTHONHOME})

if (HAVE_PYTHONHOME)
  message (STATUS "Found environment variable PYTHONHOME.")
endif (HAVE_PYTHONHOME)

## -----------------------------------------------------------------------------

foreach (python_version 2.6 2.5 2.4 2.3)

  ## Check for executable
  
  if (HAVE_PYTHONHOME)
    find_program (PYTHON_EXECUTABLE python python${python_version}
      PATHS ${HAVE_PYTHONHOME} ${HAVE_PYTHONHOME}/bin
      NO_DEFAULT_PATH
      )
  else (HAVE_PYTHONHOME)
    find_program (PYTHON_EXECUTABLE python${python_version}
      PATHS ${bin_locations}
      NO_DEFAULT_PATH
      )
  endif (HAVE_PYTHONHOME)
  
  ## Check for the PYTHON header files

  find_path (PYTHON_INCLUDES Python.h
    PATHS ${include_locations}
    PATH_SUFFIXES python${python_version}
    NO_DEFAULT_PATH
    )

  find_path (HAVE_PYCONFIG_H pyconfig.h
    PATHS ${include_locations}
    PATH_SUFFIXES python${python_version}
    NO_DEFAULT_PATH
    )

  ## Check for the NUMPY header files

  find_path (NUMPY_INCLUDES_tmp numpy/arrayobject.h
    PATHS ${lib_locations} ${include_locations}
    PATH_SUFFIXES
    python
    python${python_version}
    python${python_version}/site-packages/numpy
    python${python_version}/site-packages/numpy/core/include
    NO_DEFAULT_PATH
    )

  if (NUMPY_INCLUDES_tmp)
    get_filename_component (NUMPY_INCLUDES ${NUMPY_INCLUDES_tmp} ABSOLUTE)
  endif (NUMPY_INCLUDES_tmp)

  ## Check for the NUM_UTIL header files and libraries

  find_path (NUM_UTIL_INCLUDES num_util/num_util.h
    PATHS ${include_locations}
    PATH_SUFFIXES
    python
    python${python_version}
    NO_DEFAULT_PATH
    )

  find_library (NUM_UTIL_LIBRARIES num_util
    PATHS ${lib_locations}
    PATH_SUFFIXES
    python
    python${python_version}
    NO_DEFAULT_PATH
    )

  ## Check for the library

  find_library (PYTHON_LIBRARIES python${python_version}
    PATHS
    ${lib_locations}
    PATH_SUFFIXES
    python${python_version}/config
    NO_DEFAULT_PATH
    )

  # check if components have been found
  if (NOT PYTHON_INCLUDES OR NOT PYTHON_LIBRARIES)
    if (NOT PYTHON_FIND_QUIETLY)
      message (STATUS "No consistent set of files found for Python ${python_version}")
    endif (NOT PYTHON_FIND_QUIETLY)
  endif (NOT PYTHON_INCLUDES OR NOT PYTHON_LIBRARIES)
endforeach (python_version)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

## Python itself

if (PYTHON_INCLUDES AND PYTHON_LIBRARIES)
  set (HAVE_PYTHON TRUE)
else (PYTHON_INCLUDES AND PYTHON_LIBRARIES)
  set (HAVE_PYTHON FALSE)
  if (NOT PYTHON_FIND_QUIETLY)
    if (NOT PYTHON_INCLUDES)
      message (STATUS "Unable to find PYTHON header files!")
    endif (NOT PYTHON_INCLUDES)
    if (NOT PYTHON_LIBRARIES)
      message (STATUS "Unable to find PYTHON library files!")
    endif (NOT PYTHON_LIBRARIES)
  endif (NOT PYTHON_FIND_QUIETLY)
endif (PYTHON_INCLUDES AND PYTHON_LIBRARIES)

## NumUtil

if (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES)
  set (HAVE_NUM_UTIL TRUE)
else (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES) 
  set (HAVE_NUM_UTIL FALSE)
endif (NUM_UTIL_INCLUDES AND NUM_UTIL_LIBRARIES) 

## Feedback

if (HAVE_PYTHON)
  if (NOT PYTHON_FIND_QUIETLY)
    message (STATUS "Found components for PYTHON")
    message (STATUS "PYTHON_EXECUTABLE  = ${PYTHON_EXECUTABLE}")
    message (STATUS "PYTHON_INCLUDES    = ${PYTHON_INCLUDES}")
    message (STATUS "PYTHON_LIBRARIES   = ${PYTHON_LIBRARIES}")
    message (STATUS "NUMPY_INCLUDES     = ${NUMPY_INCLUDES}")
    message (STATUS "NUM_UTIL_INCLUDES  = ${NUM_UTIL_INCLUDES}")
    message (STATUS "NUM_UTIL_LIBRARIES = ${NUM_UTIL_LIBRARIES}")
  endif (NOT PYTHON_FIND_QUIETLY)
else (HAVE_PYTHON)
  if (PYTHON_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PYTHON!")
  endif (PYTHON_FIND_REQUIRED)
endif (HAVE_PYTHON)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PYTHON_INCLUDES
  PYTHON_LIBRARIES
  )
