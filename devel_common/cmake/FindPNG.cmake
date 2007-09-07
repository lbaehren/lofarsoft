##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 391 2007-06-13 09:25:11Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of PNG
#
# The following variables are set when PNG is found:
#  HAVE_PNG       = Set to true, if all components of PNG have been found.
#  PNG_INCLUDES   = Include path for the header files of PNG
#  PNG_LIBRARIES  = Link these to use PNG
#  PNG_LFGLAS     = Linker flags (optional)

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

find_path (PNG_INCLUDES png.h
  PATHS ${include_locations}
  PATH_SUFFIXES png
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (PNG_LIBRARIES png
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PNG_INCLUDES AND PNG_LIBRARIES)
  set (HAVE_PNG TRUE)
else (PNG_INCLUDES AND PNG_LIBRARIES)
  set (HAVE_PNG FALSE)
  if (NOT PNG_FIND_QUIETLY)
    if (NOT PNG_INCLUDES)
      message (STATUS "Unable to find PNG header files!")
    endif (NOT PNG_INCLUDES)
    if (NOT PNG_LIBRARIES)
      message (STATUS "Unable to find PNG library files!")
    endif (NOT PNG_LIBRARIES)
  endif (NOT PNG_FIND_QUIETLY)
endif (PNG_INCLUDES AND PNG_LIBRARIES)

if (HAVE_PNG)
  if (NOT PNG_FIND_QUIETLY)
    message (STATUS "Found components for PNG")
    message (STATUS "PNG_INCLUDES  = ${PNG_INCLUDES}")
    message (STATUS "PNG_LIBRARIES = ${PNG_LIBRARIES}")
  endif (NOT PNG_FIND_QUIETLY)
else (HAVE_PNG)
  if (PNG_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PNG!")
  endif (PNG_FIND_REQUIRED)
endif (HAVE_PNG)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PNG_INCLUDES
  PNG_LIBRARIES
  )
