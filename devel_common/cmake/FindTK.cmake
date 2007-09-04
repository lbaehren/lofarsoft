##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 391 2007-06-13 09:25:11Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of TK
#
# The following variables are set when TK is found:
#  HAVE_TK       = Set to true, if all components of TK have been found.
#  TK_INCLUDES   = Include path for the header files of TK
#  TK_LIBRARIES  = Link these to use TK
#  TK_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (lib_locations
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /sw/lib
  )

set (include_locations
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (TK_INCLUDES tk.h
  PATHS ${include_locations}
  PATH_SUFFIXES tk tk8.4 tk8.3 tcl tcl8.4 tcl8.3
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (TK_LIBRARIES tk tk8.4 tk8.3
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (TK_INCLUDES AND TK_LIBRARIES)
  set (HAVE_TK TRUE)
else (TK_INCLUDES AND TK_LIBRARIES)
  set (HAVE_TK FALSE)
  if (NOT TK_FIND_QUIETLY)
    if (NOT TK_INCLUDES)
      message (STATUS "Unable to find TK header files!")
    endif (NOT TK_INCLUDES)
    if (NOT TK_LIBRARIES)
      message (STATUS "Unable to find TK library files!")
    endif (NOT TK_LIBRARIES)
  endif (NOT TK_FIND_QUIETLY)
endif (TK_INCLUDES AND TK_LIBRARIES)

if (HAVE_TK)
  if (NOT TK_FIND_QUIETLY)
    message (STATUS "Found components for TK")
    message (STATUS "TK_INCLUDES  = ${TK_INCLUDES}")
    message (STATUS "TK_LIBRARIES = ${TK_LIBRARIES}")
  endif (NOT TK_FIND_QUIETLY)
else (HAVE_TK)
  if (TK_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find TK!")
  endif (TK_FIND_REQUIRED)
endif (HAVE_TK)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  TK_INCLUDES
  TK_LIBRARIES
  )
