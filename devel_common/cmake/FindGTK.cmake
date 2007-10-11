##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 815 2007-09-21 09:18:08Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of GTK
#
# The following variables are set when GTK is found:
#  HAVE_GTK       = Set to true, if all components of GTK have been found.
#  GTK_INCLUDES   = Include path for the header files of GTK
#  GTK_LIBRARIES  = Link these to use GTK
#  GTK_LFGLAS     = Linker flags (optional)

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

find_path (GTK_INCLUDES gtk.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  gtk
  gtk-2.0
  gtk-2.0/gtk
  NO_DEFAULT_PATH
  )

## adjust the include path

string (REGEX REPLACE "include/gtk-2.0/gtk" "include/gtk-2.0" GTK_INCLUDES ${GTK_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

find_library (GTK_LIBRARIES gtk
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
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

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GTK_INCLUDES
  GTK_LIBRARIES
  )
