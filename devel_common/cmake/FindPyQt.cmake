# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
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

# - Check for the presence of PYQT
#
# The following variables are set when PYQT is found:
#  HAVE_PYQT       = Set to true, if all components of PYQT have been found.
#  PYQT_INCLUDES   = Include path for the header files of PYQT
#  PYQT_LIBRARIES  = Link these to use PYQT
#  PYQT_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## Required related packages 

if (PYQT_FIND_QUIETLY)
  set (PYTHON_FIND_QUIETLY YES)
endif (PYQT_FIND_QUIETLY)

include (FindPython)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PYQT_INCLUDES PyQt4.api
  PATHS ${include_locations}
  PATH_SUFFIXES python${PYTHON_VERSION}/api/python
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

## Remove the standard library prefix

set (TMP_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})
set (CMAKE_FIND_LIBRARY_PREFIXES "")

## Search for the libraries

foreach (_lib Qt QtCore QtDesigner QtGui QtHelper QtNetwork QtTest)
  ## convert to upper-case
  string (TOUPPER ${_lib} _var)
  ## locate library
  find_library (PYQT_${_var}_LIBRARY ${_lib}
    PATHS ${lib_locations}
    PATH_SUFFIXES python${PYTHON_VERSION}/site-packages/PyQt4
    NO_DEFAULT_PATH
    )
  ## augment list of libraries
  if (PYQT_${_var}_LIBRARY)
    list (APPEND PYQT_LIBRARIES ${PYQT_${_var}_LIBRARY})
  endif (PYQT_${_var}_LIBRARY)
endforeach (_lib)

## adjust the ordering

if (PYQT_LIBRARIES)
  list (REVERSE PYQT_LIBRARIES)
endif (PYQT_LIBRARIES)

## Reinstate the library prefix 

set (CMAKE_FIND_LIBRARY_PREFIXES ${TMP_FIND_LIBRARY_PREFIXES} CACHE STRING
  "Library prefixes"
  FORCE
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PYQT_INCLUDES AND PYQT_LIBRARIES)
  set (HAVE_PYQT TRUE)
else (PYQT_INCLUDES AND PYQT_LIBRARIES)
  set (HAVE_PYQT FALSE)
  if (NOT PYQT_FIND_QUIETLY)
    if (NOT PYQT_INCLUDES)
      message (STATUS "Unable to find PyQt header files!")
    endif (NOT PYQT_INCLUDES)
    if (NOT PYQT_LIBRARIES)
      message (STATUS "Unable to find PyQt library files!")
    endif (NOT PYQT_LIBRARIES)
  endif (NOT PYQT_FIND_QUIETLY)
endif (PYQT_INCLUDES AND PYQT_LIBRARIES)

if (HAVE_PYQT)
  if (NOT PYQT_FIND_QUIETLY)
    message (STATUS "Found components for PyQt")
    message (STATUS "PYQT_INCLUDES  = ${PYQT_INCLUDES}")
    message (STATUS "PYQT_LIBRARIES = ${PYQT_LIBRARIES}")
  endif (NOT PYQT_FIND_QUIETLY)
else (HAVE_PYQT)
  if (PYQT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PyQt!")
  endif (PYQT_FIND_REQUIRED)
endif (HAVE_PYQT)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PYQT_INCLUDES
  PYQT_LIBRARIES
  PYQT_QT_LIBRARY
  PYQT_QTCORE_LIBRARY
  PYQT_QTDESIGNER_LIBRARY
  )
