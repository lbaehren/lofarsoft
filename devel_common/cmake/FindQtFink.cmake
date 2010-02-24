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

# - Check for the presence of QT
#
# The following variables are set when QT is found:
#  HAVE_QT            = Set to true, if all components of QT have been found.
#  QT_INCLUDES        = Include path for the header files of QT
#  HAVE_QT4_QWIDGET   = Include path to the QWidget header file
#  QT_LIBRARIES       = Link these to use QT
#  QT4_QTCORE_LIBRARY = Path to the QtCore library
#  QT4_QTGUI_LIBRARY  = Path to the QtGui library
#  QT_LFLAGS          = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (QT4_INCLUDES QTimer QMouseEvent
  PATHS ${include_locations} /usr/lib /sw/lib
  PATH_SUFFIXES qt4-mac qt4-mac/include qt4-x11 qt4-x11/include
  )

find_path (HAVE_QT4_QWIDGET QWidget
  PATHS ${include_locations} /usr/lib /sw/lib /opt/aips++/local/include
  PATH_SUFFIXES qt4-mac/include/QtGui qt4-x11/include/QtGui QtGui
  )
if (HAVE_QT4_QWIDGET)
  list (APPEND QT4_INCLUDES ${HAVE_QT4_QWIDGET})
endif (HAVE_QT4_QWIDGET)

## -----------------------------------------------------------------------------
## Check for the library

## QtCore

find_library (QT4_QTCORE_LIBRARY QtCore
  PATHS ${lib_locations}
  PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
  )
if (QT4_QTCORE_LIBRARY)
  list (APPEND QT4_LIBRARIES ${QT4_QTCORE_LIBRARY})
endif (QT4_QTCORE_LIBRARY)

## QtGui

find_library (QT4_QTGUI_LIBRARY QtGui
  PATHS ${lib_locations}
  PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
  )
if (QT4_QTGUI_LIBRARY)
  list (APPEND QT4_LIBRARIES ${QT4_QTGUI_LIBRARY})
endif (QT4_QTGUI_LIBRARY)

## QtDesigner

find_library (QT4_QTDESIGNER_LIBRARY QtDesigner
  PATHS ${lib_locations}
  PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
  )
if (QT4_QTDESIGNER_LIBRARY)
  list (APPEND QT4_LIBRARIES ${QT4_QTDESIGNER_LIBRARY})
endif (QT4_QTDESIGNER_LIBRARY)

## Adjust the ordering for the statement passed to the linker

if (QT4_LIBRARIES)
  list(REVERSE QT4_LIBRARIES)
endif (QT4_LIBRARIES)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (QT_INCLUDES AND QT_LIBRARIES)
  set (HAVE_QT TRUE)
else (QT_INCLUDES AND QT_LIBRARIES)
  set (HAVE_QT FALSE)
  if (NOT QT_FIND_QUIETLY)
    if (NOT QT_INCLUDES)
      message (STATUS "Unable to find QT header files!")
    endif (NOT QT_INCLUDES)
    if (NOT QT_LIBRARIES)
      message (STATUS "Unable to find QT library files!")
    endif (NOT QT_LIBRARIES)
  endif (NOT QT_FIND_QUIETLY)
endif (QT_INCLUDES AND QT_LIBRARIES)

if (HAVE_QT)
  if (NOT QT_FIND_QUIETLY)
    message (STATUS "Found components for QT")
    message (STATUS "QT_INCLUDES  = ${QT_INCLUDES}")
    message (STATUS "QT_LIBRARIES = ${QT_LIBRARIES}")
  endif (NOT QT_FIND_QUIETLY)
else (HAVE_QT)
  if (QT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find QT!")
  endif (QT_FIND_REQUIRED)
endif (HAVE_QT)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  QT_INCLUDES
  QT_LIBRARIES
  HAVE_QT4_QWIDGET
  QT4_QTCORE_LIBRARY
  QT4_QTGUI_LIBRARY
  QT4_QTDESIGNER_LIBRARY
  )
