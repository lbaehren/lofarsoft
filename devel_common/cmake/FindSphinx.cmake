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

# - Check for the presence of SPHINX
#
# The following variables are set when SPHINX is found:
#  HAVE_SPHINX               = Set to true, if all components of SPHINX have been
#                             found.
#  SPHINX_EXECUTABLE          = The sphinx executable
#                             package
#  SPHINX_MAJOR_VERSION      = Sphinx major version
#  SPHINX_MINOR_VERSION      = Sphinx minor version

## -----------------------------------------------------------------------------
## Find python

find_package (Python)

## -----------------------------------------------------------------------------
## Check for executables

find_program (SPHINX_EXECUTABLE sphinx)
if (NOT SPHINX_EXECUTABLE)
  find_program (SPHINX_EXECUTABLE sphinx-build)
endif (NOT SPHINX_EXECUTABLE)

## -----------------------------------------------------------------------------
## Try to determine version

if (SPHINX_EXECUTABLE)
  ## Run Python and import Sphinx to print the version information
  execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "import sphinx; print sphinx.__version__"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE sphinx_version_test_result
    OUTPUT_VARIABLE sphinx_version_test_output
    ERROR_VARIABLE sphinx_version_test_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (SPHINX_EXECUTABLE)

if (sphinx_version_test_output)
  ## copy the output from the Python prompt
  set (SPHINX_API_VERSION ${sphinx_version_test_output})
  ## Extract major version
  string (REGEX MATCH "[0-9]" SPHINX_MAJOR_VERSION ${sphinx_version_test_output})
  ## Extract minor version
  string (REGEX REPLACE "${SPHINX_MAJOR_VERSION}." "" sphinx_version_test_output ${sphinx_version_test_output})
  string (REGEX MATCH "[0-9]" SPHINX_MINOR_VERSION ${sphinx_version_test_output})
endif (sphinx_version_test_output)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (SPHINX_EXECUTABLE AND SPHINX_MAJOR_VERSION AND SPHINX_MINOR_VERSION)
  set (HAVE_SPHINX  TRUE)
  set (SPHINX_FOUND TRUE)
else (SPHINX_EXECUTABLE AND SPHINX_MAJOR_VERSION AND SPHINX_MINOR_VERSION)
  set (HAVE_SPHINX  FALSE)
  set (SPHINX_FOUND FALSE)
endif (SPHINX_EXECUTABLE AND SPHINX_MAJOR_VERSION AND SPHINX_MINOR_VERSION)

if (SPHINX_FOUND)
  if (NOT SPHINX_FIND_QUIETLY)
    message (STATUS "Found Sphinx")
  endif (NOT SPHINX_FIND_QUIETLY)
else (SPHINX_FOUND)
  if (SPHINX_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Sphinx!")
  endif (SPHINX_FIND_REQUIRED)
endif (SPHINX_FOUND)

