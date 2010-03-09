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
#  QT_MOC_EXECUTABLE      -- Path to the moc application program
#  QT_QMAKE_EXECUTABLE    -- Path to the qmake application program
#  QT_INCLUDES            -- Common Qt include directory
#  QT_QTCORE_INCLUDES     -- QtCore header files
#  QT_QTGUI_INCLUDES      -- QtGui header files 
#  QT_QTCORE_LIBRARY      -- QtCore library
#  QT_QTGUI_LIBRARY       -- QtGui libray
#  QT_QTDESIGNER_LIBRARY  -- QtDesigner library

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Executables

## moc

if (NOT QT_MOC_EXECUTABLE)
  find_program (QT_MOC_EXECUTABLE moc ${bin_locations})
endif (NOT QT_MOC_EXECUTABLE)

## qmake

if (NOT QT_QMAKE_EXECUTABLE)
  find_program (QT_QMAKE_EXECUTABLE qmake ${bin_locations})
endif (NOT QT_QMAKE_EXECUTABLE)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (QT_INCLUDES QtCore/QTimerEvent QtCore/QResource
  PATHS ${include_locations} /sw/lib
  PATH_SUFFIXES
  qt4-mac
  qt4-mac/include
  qt4-x11
  qt4-x11/include
  )

find_path (QT_QTCORE_INCLUDES QTimerEvent QResource
  PATHS ${include_locations} /sw/lib
  PATH_SUFFIXES
  qt4-mac
  qt4-mac/include
  qt4-mac/include/QtCore
  qt4-x11
  qt4-x11/include
  qt4-x11/include/QtCore
  )
if (QT_QTCORE_INCLUDES)
  list (APPEND QT_INCLUDES ${QT_QTCORE_INCLUDES})
endif (QT_QTCORE_INCLUDES)

find_path (QT_QTGUI_INCLUDES QAction QCloseEvent QMenu
  PATHS ${include_locations} /sw/lib
  PATH_SUFFIXES
  qt4-mac
  qt4-mac/include
  qt4-mac/include/QtGui
  qt4-x11
  qt4-x11/include
  qt4-x11/include/QtGui
  )
if (QT_QTGUI_INCLUDES)
  list (APPEND QT_INCLUDES ${QT_QTGUI_INCLUDES})
endif (QT_QTGUI_INCLUDES)

## -----------------------------------------------------------------------------
## Check for the library

foreach (_lib
    QtCore
    QtGui
    QtDesigner
    QtMotif
    QtMultimedia
    QtOpenGL
    QtXml
    )
  ## variable name to be used internally
  string (TOUPPER ${_lib} _var)
  ## search for the library
  if (NOT QT_${_var}_LIBRARY)
    find_library (QT_${_var}_LIBRARY ${_lib}
      PATHS ${lib_locations} /sw/lib
      PATH_SUFFIXES
      qt4-mac
      qt4-mac/lib
      qt4-x11
      qt4-x11/lib
      NO_DEFAULT_PATH
      )
    ## append to list of libraries
    if (QT_${_var}_LIBRARY)
      list (APPEND QT_LIBRARIES ${QT_${_var}_LIBRARY})
    endif (QT_${_var}_LIBRARY)
  endif (NOT QT_${_var}_LIBRARY)
endforeach (_lib)

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
