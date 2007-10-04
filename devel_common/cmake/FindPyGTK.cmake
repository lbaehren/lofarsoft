##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 815 2007-09-21 09:18:08Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of PyGTK [www.pygtk.org]
#
# The following variables are set when PyGTK is found:
#  HAVE_PyGTK       = Set to true, if all components of PyGTK have been found.
#  PyGTK_INCLUDES   = Include path for the header files of PyGTK
#  PyGTK_LIBRARIES  = Link these to use PyGTK
#  PyGTK_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Relevance for other Python modules

# In your build environment, see if these directories show up with
#
#   > pkg-config --cflags-only-I pygtk-2.0
#
# That is what mpl uses to find your pygtk headers.  If not, set your
# PKG_CONFIG_PATH environment variable accordingly, and make sure there
# is a pygtk-2.0.pc file in that directory.  On my system it is in
#
#   /usr/lib/pkgconfig/pygtk-2.0.pc
#
# and yours will probably be /usr/local/lib/pkgconfig
#
# pygtk-2.0.pc

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  ## local installation
  ./../release/bin
  ./../../release/bin
  ## system-wide installation
  /usr/local/bin
  /usr/bin
  /usr/X11R6/bin
  /opt/bin
  /opt/local/bin
  /sw/bin
  )

set (lib_locations
  ## local installation
  ./../release/lib
  ./../../release/lib
  ## system-wide installation
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /opt/lib
  /opt/local/lib
  /sw/lib
  )

set (include_locations
  ./../release/include
  ./../../release/include
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PyGTK_INCLUDES pygtk.h
  PATHS ${include_locations}
  PATH_SUFFIXES pygtk pygtk-2.0
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (PyGTK_LIBRARIES pygtk
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PyGTK_INCLUDES AND PyGTK_LIBRARIES)
  set (HAVE_PyGTK TRUE)
else (PyGTK_INCLUDES AND PyGTK_LIBRARIES)
  set (HAVE_PyGTK FALSE)
  if (NOT PyGTK_FIND_QUIETLY)
    if (NOT PyGTK_INCLUDES)
      message (STATUS "Unable to find PyGTK header files!")
    endif (NOT PyGTK_INCLUDES)
    if (NOT PyGTK_LIBRARIES)
      message (STATUS "Unable to find PyGTK library files!")
    endif (NOT PyGTK_LIBRARIES)
  endif (NOT PyGTK_FIND_QUIETLY)
endif (PyGTK_INCLUDES AND PyGTK_LIBRARIES)

if (HAVE_PyGTK)
  if (NOT PyGTK_FIND_QUIETLY)
    message (STATUS "Found components for PyGTK")
    message (STATUS "PyGTK_INCLUDES  = ${PyGTK_INCLUDES}")
    message (STATUS "PyGTK_LIBRARIES = ${PyGTK_LIBRARIES}")
  endif (NOT PyGTK_FIND_QUIETLY)
else (HAVE_PyGTK)
  if (PyGTK_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PyGTK!")
  endif (PyGTK_FIND_REQUIRED)
endif (HAVE_PyGTK)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PyGTK_INCLUDES
  PyGTK_LIBRARIES
  )
