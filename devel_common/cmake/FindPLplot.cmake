##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 815 2007-09-21 09:18:08Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of PLplot
#
# The following variables are set when PLPLOT is found:
#  HAVE_PLPLOT       = Set to true, if all components of PLplot have been found.
#  PLPLOT_INCLUDES   = Include path for the header files of PLplot
#  PLPLOT_LIBRARIES  = Link these to use PLplot
#  PLPLOT_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PLPLOT_INCLUDES plplot.h plplotcanvas.h
  PATHS ${include_locations}
  PATH_SUFFIXES plplot
  NO_DEFAULT_PATH
  )

if (PLPLOT_INCLUDES)
  string (REGEX REPLACE "include/plplot" "include" PLPLOT_INCLUDES ${PLPLOT_INCLUDES})
endif (PLPLOT_INCLUDES)

## -----------------------------------------------------------------------------
## Check for the library

## [1] 

find_library (PLPLOT_plplotd plplotd
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (PLPLOT_plplotd)
  list (APPEND PLPLOT_LIBRARIES ${PLPLOT_plplotd})
endif (PLPLOT_plplotd)

## [2] 

find_library (PLPLOT_plplotcxxd plplotcxxd
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (PLPLOT_plplotcxxd)
  list (APPEND PLPLOT_LIBRARIES ${PLPLOT_plplotcxxd})
endif (PLPLOT_plplotcxxd)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)
  set (HAVE_PLPLOT TRUE)
else (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)
  set (HAVE_PLPLOT FALSE)
  if (NOT PLPLOT_FIND_QUIETLY)
    if (NOT PLPLOT_INCLUDES)
      message (STATUS "Unable to find PLPLOT header files!")
    endif (NOT PLPLOT_INCLUDES)
    if (NOT PLPLOT_LIBRARIES)
      message (STATUS "Unable to find PLPLOT library files!")
    endif (NOT PLPLOT_LIBRARIES)
  endif (NOT PLPLOT_FIND_QUIETLY)
endif (PLPLOT_INCLUDES AND PLPLOT_LIBRARIES)

if (HAVE_PLPLOT)
  if (NOT PLPLOT_FIND_QUIETLY)
    message (STATUS "Found components for PLPLOT")
    message (STATUS "PLPLOT_INCLUDES  = ${PLPLOT_INCLUDES}")
    message (STATUS "PLPLOT_LIBRARIES = ${PLPLOT_LIBRARIES}")
  endif (NOT PLPLOT_FIND_QUIETLY)
else (HAVE_PLPLOT)
  if (PLPLOT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PLPLOT!")
  endif (PLPLOT_FIND_REQUIRED)
endif (HAVE_PLPLOT)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PLPLOT_INCLUDES
  PLPLOT_LIBRARIES
  )
