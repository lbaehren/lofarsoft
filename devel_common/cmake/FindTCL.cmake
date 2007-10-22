##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 391 2007-06-13 09:25:11Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of TCL
#
# The following variables are set when TCL is found:
#  HAVE_TCL       = Set to true, if all components of TCL have been found.
#  TCL_INCLUDES   = Include path for the header files of TCL
#  TCL_LIBRARIES  = Link these to use TCL
#  TCL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

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
  ## local installation
  ./../release/include
  ./../../release/include
  ## system-wide installation
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (TCL_INCLUDES tcl.h
  PATHS ${include_locations}
  PATH_SUFFIXES tcl tcl8.4 tcl8.3
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (TCL_LIBRARIES tcl tcl8.4 tcl8.3
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (TCL_INCLUDES AND TCL_LIBRARIES)
  set (HAVE_TCL TRUE)
else (TCL_INCLUDES AND TCL_LIBRARIES)
  set (HAVE_TCL FALSE)
  if (NOT TCL_FIND_QUIETLY)
    if (NOT TCL_INCLUDES)
      message (STATUS "Unable to find TCL header files!")
    endif (NOT TCL_INCLUDES)
    if (NOT TCL_LIBRARIES)
      message (STATUS "Unable to find TCL library files!")
    endif (NOT TCL_LIBRARIES)
  endif (NOT TCL_FIND_QUIETLY)
endif (TCL_INCLUDES AND TCL_LIBRARIES)

if (HAVE_TCL)
  if (NOT TCL_FIND_QUIETLY)
    message (STATUS "Found components for TCL")
    message (STATUS "TCL_INCLUDES  = ${TCL_INCLUDES}")
    message (STATUS "TCL_LIBRARIES = ${TCL_LIBRARIES}")
  endif (NOT TCL_FIND_QUIETLY)
else (HAVE_TCL)
  if (TCL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find TCL!")
  endif (TCL_FIND_REQUIRED)
endif (HAVE_TCL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  TCL_INCLUDES
  TCL_LIBRARIES
  )
