##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 391 2007-06-13 09:25:11Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of XAW
#
# The following variables are set when XAW is found:
#  HAVE_XAW       = Set to true, if all components of XAW have been found.
#  XAW_INCLUDES   = Include path for the header files of XAW
#  XAW_LIBRARIES  = Link these to use XAW
#  XAW_LFGLAS     = Linker flags (optional)

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

find_path (XAW_INCLUDES XawInit.h XawImP.h
  PATHS ${include_locations}
  PATH_SUFFIXES X11 X11/Xaw
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (XAW_LIBRARIES Xaw
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (XAW_INCLUDES AND XAW_LIBRARIES)
  set (HAVE_XAW TRUE)
else (XAW_INCLUDES AND XAW_LIBRARIES)
  set (HAVE_XAW FALSE)
  if (NOT XAW_FIND_QUIETLY)
    if (NOT XAW_INCLUDES)
      message (STATUS "Unable to find XAW header files!")
    endif (NOT XAW_INCLUDES)
    if (NOT XAW_LIBRARIES)
      message (STATUS "Unable to find XAW library files!")
    endif (NOT XAW_LIBRARIES)
  endif (NOT XAW_FIND_QUIETLY)
endif (XAW_INCLUDES AND XAW_LIBRARIES)

if (HAVE_XAW)
  if (NOT XAW_FIND_QUIETLY)
    message (STATUS "Found components for XAW")
    message (STATUS "XAW_INCLUDES  = ${XAW_INCLUDES}")
    message (STATUS "XAW_LIBRARIES = ${XAW_LIBRARIES}")
  endif (NOT XAW_FIND_QUIETLY)
else (HAVE_XAW)
  if (XAW_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find XAW!")
  endif (XAW_FIND_REQUIRED)
endif (HAVE_XAW)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  XAW_INCLUDES
  XAW_LIBRARIES
  )
