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

# - Check for the presence of GTK
#
# The following variables are set when GTK is found:
#  HAVE_GTK       = Set to true, if all components of GTK have been found.
#  GTK_INCLUDES   = Include path for the header files of GTK
#  GTK_LIBRARIES  = Link these to use GTK
#  GTK_LFGLAS     = Linker flags (optional)

if (NOT FIND_GTK_CMAKE)
  
  set (FIND_GTK_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (GTK_INCLUDES gtk.h
    PATHS ${include_locations}
    PATH_SUFFIXES
    gtk
    gtk-2.0
    gtk-2.0/gtk
    NO_DEFAULT_PATH
    )
  
  ## adjust the include path
  
  string (REGEX REPLACE
    "include/gtk-2.0/gtk" "include/gtk-2.0"
    GTK_INCLUDES ${GTK_INCLUDES}
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  ##
  ## To get a list of libraries which need to be included into the linking stage,
  ## run:
  ##      pkg-config --cflags --libs gtk+-2.0
  ##
  
  ## [1] Locate the basic GTK library first
  
  find_library (GTK_LIBRARIES gtk gtk-2.0
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ## [2] Check for libgtk-x11
  
  find_library (libgtk_x11 gtk-x11 gtk-x11-2.0
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  if (libgtk_x11)
    list (APPEND GTK_LIBRARIES ${libgtk_x11})
  endif (libgtk_x11)
  
  ## [3] Check for libgdk-x11
  
  find_library (libgdk_x11 gdk-x11 gdk-x11-2.0
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  if (libgdk_x11)
    list (APPEND GTK_LIBRARIES ${libgdk_x11})
  endif (libgdk_x11)
  
  ## [4] Check for libatk
  
  find_library (libatk atk atk-1.0 atk-2.0
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  if (libatk)
    list (APPEND GTK_LIBRARIES ${libatk})
  endif (libatk)
  
  ## Once we are done, rearrange the order of the libraries in the list
  
  #if (GTK_LIBRARIES)
  #  list (REVERSE GTK_LIBRARIES)
  #endif (GTK_LIBRARIES)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (GTK_INCLUDES AND GTK_LIBRARIES)
    set (HAVE_GTK TRUE)
  else (GTK_INCLUDES AND GTK_LIBRARIES)
    set (HAVE_GTK FALSE)
    if (NOT GTK_FIND_QUIETLY)
      if (NOT GTK_INCLUDES)
	message (STATUS "Unable to find PyGtk header files!")
      endif (NOT GTK_INCLUDES)
      if (NOT GTK_LIBRARIES)
	message (STATUS "Unable to find PyGtk library files!")
      endif (NOT GTK_LIBRARIES)
    endif (NOT GTK_FIND_QUIETLY)
  endif (GTK_INCLUDES AND GTK_LIBRARIES)
  
  if (HAVE_GTK)
    if (NOT GTK_FIND_QUIETLY)
      message (STATUS "Found components for PyGtk")
      message (STATUS "GTK_INCLUDES  = ${GTK_INCLUDES}")
      message (STATUS "GTK_LIBRARIES = ${GTK_LIBRARIES}")
    endif (NOT GTK_FIND_QUIETLY)
  else (HAVE_GTK)
    if (GTK_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find GTK!")
    endif (GTK_FIND_REQUIRED)
  endif (HAVE_GTK)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    GTK_INCLUDES
    GTK_LIBRARIES
    )
  
endif (NOT FIND_GTK_CMAKE)
