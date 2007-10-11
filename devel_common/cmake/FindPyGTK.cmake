##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 815 2007-09-21 09:18:08Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of PYGTK [www.pygtk.org]
#
# The following variables are set when PYGTK is found:
#  HAVE_PYGTK       = Set to true, if all components of PYGTK have been found.
#  PYGTK_INCLUDES   = Include path for the header files of PYGTK
#  PYGTK_LIBRARIES  = Link these to use PYGTK
#  PYGTK_LFGLAS     = Linker flags (optional)

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

find_path (PYGTK_INCLUDES pygtk.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  pygtk
  pygtk-2.0
  pygtk-2.0/pygtk
  NO_DEFAULT_PATH
  )

string (REGEX REPLACE "include/pygtk-2.0/pygtk" "include/pygtk-2.0" PYGTK_INCLUDES ${PYGTK_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

find_library (PYGTK_LIBRARIES pygtk
  PATHS ${lib_locations}
  PATH_SUFFIXES
  pygtk
  pygtk-2.0
  pygtk-2.0/pygtk
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PYGTK_INCLUDES AND PYGTK_LIBRARIES)
  set (HAVE_PYGTK TRUE)
else (PYGTK_INCLUDES AND PYGTK_LIBRARIES)
  set (HAVE_PYGTK FALSE)
  if (NOT PYGTK_FIND_QUIETLY)
    if (NOT PYGTK_INCLUDES)
      message (STATUS "Unable to find PyGtk header files!")
    endif (NOT PYGTK_INCLUDES)
    if (NOT PYGTK_LIBRARIES)
      message (STATUS "Unable to find PyGtk library files!")
    endif (NOT PYGTK_LIBRARIES)
  endif (NOT PYGTK_FIND_QUIETLY)
endif (PYGTK_INCLUDES AND PYGTK_LIBRARIES)

if (HAVE_PYGTK)
  if (NOT PYGTK_FIND_QUIETLY)
    message (STATUS "Found components for PyGtk")
    message (STATUS "PYGTK_INCLUDES  = ${PYGTK_INCLUDES}")
    message (STATUS "PYGTK_LIBRARIES = ${PYGTK_LIBRARIES}")
  endif (NOT PYGTK_FIND_QUIETLY)
else (HAVE_PYGTK)
  if (PYGTK_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PYGTK!")
  endif (PYGTK_FIND_REQUIRED)
endif (HAVE_PYGTK)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PYGTK_INCLUDES
  PYGTK_LIBRARIES
  )
