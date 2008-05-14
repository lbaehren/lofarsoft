##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 391 2007-06-13 09:25:11Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of GIF
#
# The following variables are set when GIF is found:
#  HAVE_GIF       = Set to true, if all components of GIF have been found.
#  GIF_INCLUDES   = Include path for the header files of GIF
#  GIF_LIBRARIES  = Link these to use GIF
#  GIF_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GIF_INCLUDES gif.h
  PATHS ${include_locations}
  PATH_SUFFIXES gif
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (GIF_LIBRARIES gif
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GIF_INCLUDES AND GIF_LIBRARIES)
  set (HAVE_GIF TRUE)
else (GIF_INCLUDES AND GIF_LIBRARIES)
  set (HAVE_GIF FALSE)
  if (NOT GIF_FIND_QUIETLY)
    if (NOT GIF_INCLUDES)
      message (STATUS "Unable to find GIF header files!")
    endif (NOT GIF_INCLUDES)
    if (NOT GIF_LIBRARIES)
      message (STATUS "Unable to find GIF library files!")
    endif (NOT GIF_LIBRARIES)
  endif (NOT GIF_FIND_QUIETLY)
endif (GIF_INCLUDES AND GIF_LIBRARIES)

if (HAVE_GIF)
  if (NOT GIF_FIND_QUIETLY)
    message (STATUS "Found components for GIF")
    message (STATUS "GIF_INCLUDES  = ${GIF_INCLUDES}")
    message (STATUS "GIF_LIBRARIES = ${GIF_LIBRARIES}")
  endif (NOT GIF_FIND_QUIETLY)
else (HAVE_GIF)
  if (GIF_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GIF!")
  endif (GIF_FIND_REQUIRED)
endif (HAVE_GIF)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GIF_INCLUDES
  GIF_LIBRARIES
  )
