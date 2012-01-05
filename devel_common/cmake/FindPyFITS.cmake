# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2012                                                        |
# |   John Swinbank <swinbank@transientskp.org>                                 |
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

# - Check for the presence of PyFITS
#
# The following variables are set when PyFITS is found:
#  HAVE_PYFITS              = Set to true if PyFITS is found.
#  PYFITS_MAJOR_VERSION     = PyFITS major version
#  PYFITS_MINOR_VERSION     = PyFITS minor version
#  PYFITS_RELEASE_VERSION   = PyFITS release version

if (PYFITS_FIND_QUIETLY)
  set (PYTHON_FIND_QUIETLY TRUE)
endif (PYFITS_FIND_QUIETLY)

include (FindPython)

execute_process (
  COMMAND ${PYTHON_EXECUTABLE} -c "import pyfits; print pyfits.__version__"
  RESULT_VARIABLE pyfits_test_result
  OUTPUT_VARIABLE pyfits_test_output
  ERROR_VARIABLE pyfits_test_error
  )

if (pyfits_test_result EQUAL 0)
  set (HAVE_PYFITS TRUE)
  string (REGEX MATCH "[0-9]" PYFITS_MAJOR_VERSION ${pyfits_test_output})
  string (REGEX REPLACE "${PYFITS_MAJOR_VERSION}." "" pyfits_test_output ${pyfits_test_output})
  string (REGEX MATCH "[0-9]" PYFITS_MINOR_VERSION ${pyfits_test_output})
  string (REGEX REPLACE "${PYFITS_MINOR_VERSION}." "" pyfits_test_output ${pyfits_test_output})
  if (PYFITS_RELEASE_VERSION)
    string (REGEX MATCH "[0-9]" PYFITS_RELEASE_VERSION ${pyfits_test_output})
  endif (PYFITS_RELEASE_VERSION)
  if (NOT PYFITS_FIND_QUIETLY)
    message (STATUS "Found PyFITS v${PYFITS_MAJOR_VERSION}.${PYFITS_MINOR_VERSION}.${PYFITS_RELEASE_VERSION}")
  endif (NOT PYFITS_FIND_QUIETLY)
else (pyfits_test_result EQUAL 0)
  set (HAVE_PYFITS FALSE)
  if (NOT PYFITS_FIND_QUIETLY)
    message (STATUS "PyFITS not available; ${PYTHON_EXECUTABLE} said:")
    message (STATUS "${pyfits_test_error}")
  endif (NOT PYFITS_FIND_QUIETLY)
endif (pyfits_test_result EQUAL 0)

set (PYFITS_FOUND ${HAVE_PYFITS})
