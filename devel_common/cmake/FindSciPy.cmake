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

# - Check for the presence of SCIPY
#
# The following variables are set when SCIPY is found:
#  HAVE_SCIPY               = Set to true, if all components of SCIPY have
#                             been found.
#
#  SCIPY_MAJOR_VERSION      = SciPy major version
#  SCIPY_MINOR_VERSION      = SciPy minor version
#  SCIPY_RELEASE_VERSION    = SciPy release/patch version

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

if (SCIPY_FIND_QUIETLY)
  set (PYTHON_FIND_QUIETLY TRUE)
endif (SCIPY_FIND_QUIETLY)

include (FindPython)

## -----------------------------------------------------------------------------
## As the shared libraries of a Python module typically do not contain the
## usual prefix, we need to remove it while searching for the SciPy libraries.
## In order however not to affect other CMake modules we need to swap back in the
## original prefixes once the end of this module is reached.

set (TMP_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})

set (CMAKE_FIND_LIBRARY_PREFIXES "" CACHE STRING
  "Library prefixes"
  FORCE
  )

## -----------------------------------------------------------------------------
## Let's assume the Python executable is smarter about finding SciPy than we
## are, and try asking it before searching ourselves.
## This is necessary to e.g. pick up the MacPorts SciPy installation, which
## ends up in /opt/local/Library/Frameworks/Python.framework ...

execute_process (
  COMMAND ${PYTHON_EXECUTABLE} -c "import scipy, os; print os.path.dirname(scipy.__file__)"
  OUTPUT_VARIABLE scipy_path
  )
if (scipy_path)
  string (STRIP ${scipy_path} scipy_search_path)
else (scipy_path)
  set (scipy_search_path ${lib_locations})
endif (scipy_path)

## -----------------------------------------------------------------------------
## Try to determine the API version

if (PYTHON_EXECUTABLE)
  ## some basic feedback
  message (STATUS "Found version.py - running Python to import module scipy.")
  ## Run Python to import module scipy and print the version information
  execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "import scipy; print scipy.__version__"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE scipy_version_test_result
    OUTPUT_VARIABLE scipy_version_test_output
    ERROR_VARIABLE scipy_version_test_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else (PYTHON_EXECUTABLE)
  message (STATUS "[SciPy] Unable to process version.py script!")
endif (PYTHON_EXECUTABLE)

if (scipy_version_test_output)
  ## copy the output from the Python prompt
  set (SCIPY_API_VERSION ${scipy_version_test_output})
  ## Extract major version
  string (REGEX MATCH "[0-9]" SCIPY_MAJOR_VERSION ${scipy_version_test_output})
  ## Extract minor version
  string (REGEX REPLACE "${SCIPY_MAJOR_VERSION}." "" scipy_version_test_output ${scipy_version_test_output})
  string (REGEX MATCH "[0-9]" SCIPY_MINOR_VERSION ${scipy_version_test_output})
  ## Extract patch version
  string (REGEX REPLACE "${SCIPY_MINOR_VERSION}." "" scipy_version_test_output ${scipy_version_test_output})
  string (REGEX MATCH "[0-9]" SCIPY_RELEASE_VERSION ${scipy_version_test_output})
endif (scipy_version_test_output)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (SCIPY_MAJOR_VERSION AND SCIPY_MINOR_VERSION AND SCIPY_RELEASE_VERSION)
  set (HAVE_SCIPY TRUE)
endif (SCIPY_MAJOR_VERSION AND SCIPY_MINOR_VERSION AND SCIPY_RELEASE_VERSION)

if (HAVE_SCIPY)
  if (NOT SCIPY_FIND_QUIETLY)
    message (STATUS "Found SciPy")
  endif (NOT SCIPY_FIND_QUIETLY)
else (HAVE_SCIPY)
  if (SCIPY_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find SciPy!")
  endif (SCIPY_FIND_REQUIRED)
endif (HAVE_SCIPY)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  SCIPY_API_VERSION
  )

## -----------------------------------------------------------------------------
## Reinstate the original value of CMAKE_FIND_LIBRARY_PREFIXES

set (CMAKE_FIND_LIBRARY_PREFIXES ${TMP_FIND_LIBRARY_PREFIXES} CACHE STRING
  "Library prefixes"
  FORCE
  )

