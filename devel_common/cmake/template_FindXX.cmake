##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of <PACKAGE>
#
# The following variables are set when <PACKAGE> is found:
#  HAVE_<PACKAGE>       = Set to true, if all components of <PACKAGE>
#                          have been found.
#  <PACKAGE>_INCLUDES   = Include path for the header files of <PACKAGE>
#  <PACKAGE>_LIBRARIES  = Link these to use <PACKAGE>
#  <PACKAGE>_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/local/bin
  /usr/bin
  /usr/X11R6/bin
  /opt/bin
  /opt/local/bin
  /sw/bin
  )

set (lib_locations
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /opt/lib
  /opt/local/lib
  /sw/lib
  )

set (include_locations
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (<PACKAGE>_INCLUDES <header file(s)>
  PATHS ${include_locations}
  PATH_SUFFIXES <optional path extension>
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (<PACKAGE>_LIBRARIES <package name>
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)
  set (HAVE_<PACKAGE> TRUE)
else (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)
  set (HAVE_<PACKAGE> FALSE)
  if (NOT <PACKAGE>_FIND_QUIETLY)
    if (NOT <PACKAGE>_INCLUDES)
      message (STATUS "Unable to find <PACKAGE> header files!")
    endif (NOT <PACKAGE>_INCLUDES)
    if (NOT <PACKAGE>_LIBRARIES)
      message (STATUS "Unable to find <PACKAGE> library files!")
    endif (NOT <PACKAGE>_LIBRARIES)
  endif (NOT <PACKAGE>_FIND_QUIETLY)
endif (<PACKAGE>_INCLUDES AND <PACKAGE>_LIBRARIES)

if (HAVE_<PACKAGE>)
  if (NOT <PACKAGE>_FIND_QUIETLY)
    message (STATUS "Found components for <PACKAGE>")
    message (STATUS "<PACKAGE>_INCLUDES  = ${<PACKAGE>_INCLUDES}")
    message (STATUS "<PACKAGE>_LIBRARIES = ${<PACKAGE>_LIBRARIES}")
  endif (NOT <PACKAGE>_FIND_QUIETLY)
else (HAVE_<PACKAGE>)
  if (<PACKAGE>_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find <PACKAGE>!")
  endif (<PACKAGE>_FIND_REQUIRED)
endif (HAVE_<PACKAGE>)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  <PACKAGE>_INCLUDES
  <PACKAGE>_LIBRARIES
  )
