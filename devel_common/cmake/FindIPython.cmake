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

# - Check for the presence of IPYTHON
#
# The following variables are set when IPYTHON is found:
#  HAVE_IPYTHON               = Set to true, if all components of IPYTHON have been
#                             found.
#  IPYTHON_EXECUTABLE          = The ipython executable
#                             package
#  IPYTHON_MAJOR_VERSION      = IPython major version
#  IPYTHON_MINOR_VERSION      = IPython minor version

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for executables

find_program (IPYTHON_EXECUTABLE ipython
  PATHS ${bin_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Try to determine version

if (IPYTHON_EXECUTABLE)
  ## Run IPython to print the version information
  execute_process (
    COMMAND ${PYTHON_EXECUTABLE} --version
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE ipython_version_test_result
    OUTPUT_VARIABLE ipython_version_test_output
    ERROR_VARIABLE ipython_version_test_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (IPYTHON_EXECUTABLE)

if (ipython_version_test_output)
  ## copy the output from the Python prompt
  set (IPYTHON_API_VERSION ${ipython_version_test_output})
  ## Extract major version
  string (REGEX MATCH "[0-9]" IPYTHON_MAJOR_VERSION ${ipython_version_test_output})
  ## Extract minor version
  string (REGEX REPLACE "${IPYTHON_MAJOR_VERSION}." "" ipython_version_test_output ${ipython_version_test_output})
  string (REGEX MATCH "[0-9]" IPYTHON_MINOR_VERSION ${ipython_version_test_output})
else (ipython_version_test_output)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (IPYTHON_EXECUTABLE AND IPYTHON_MAJOR_VERSION AND IPYTHON_MINOR_VERSION)
  set (HAVE_IPYTHON TRUE)
endif (IPYTHON_EXECUTABLE AND IPYTHON_MAJOR_VERSION AND IPYTHON_MINOR_VERSION)

if (HAVE_IPYTHON)
  if (NOT IPYTHON_FIND_QUIETLY)
    message (STATUS "Found IPython")
  endif (NOT IPYTHON_FIND_QUIETLY)
else (HAVE_IPYTHON)
  if (IPYTHON_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find IPython!")
  else (IPYTHON_FIND_REQUIRED)
    message (MESSAGE "Could not find IPython!")
  endif (IPYTHON_FIND_REQUIRED)
endif (HAVE_IPYTHON)

