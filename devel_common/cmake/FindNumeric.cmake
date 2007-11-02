##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 815 2007-09-21 09:18:08Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of Numeric
#
# The following variables are set when Numeric is found:
#  HAVE_NUMERIC       = Set to true, if all components of Numeric
#                          have been found.
#  NUMERIC_INCLUDES   = Include path for the header files of Numeric
#  NUMERIC_LIBRARIES  = Link these to use Numeric
#  NUMERIC_LFGLAS     = Linker flags (optional)

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

find_path (NUMERIC_INCLUDES arrayobject.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  python
  python/numeric
  python/Numeric
  NO_DEFAULT_PATH
  )

## most likely we need to adjust the path in order to support include via
## something like <numeric/numeric.h>

string (REGEX REPLACE "include/python/numeric" "include/python" NUMERIC_INCLUDES ${NUMERIC_INCLUDES})

string (REGEX REPLACE "include/python/Numeric" "include/python" NUMERIC_INCLUDES ${NUMERIC_INCLUDES})

## -----------------------------------------------------------------------------
## Check for the library

find_library (NUMERIC_LIBRARIES _numpy
  PATHS ${lib_locations}
  PATH_SUFFIXES
  python
  python/numeric
  python/Numeric
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (NUMERIC_INCLUDES AND NUMERIC_LIBRARIES)
  set (HAVE_NUMERIC TRUE)
else (NUMERIC_INCLUDES AND NUMERIC_LIBRARIES)
  set (HAVE_NUMERIC FALSE)
  if (NOT NUMERIC_FIND_QUIETLY)
    if (NOT NUMERIC_INCLUDES)
      message (STATUS "Unable to find NUMERIC header files!")
    endif (NOT NUMERIC_INCLUDES)
    if (NOT NUMERIC_LIBRARIES)
      message (STATUS "Unable to find NUMERIC library files!")
    endif (NOT NUMERIC_LIBRARIES)
  endif (NOT NUMERIC_FIND_QUIETLY)
endif (NUMERIC_INCLUDES AND NUMERIC_LIBRARIES)

if (HAVE_NUMERIC)
  if (NOT NUMERIC_FIND_QUIETLY)
    message (STATUS "Found components for NUMERIC")
    message (STATUS "NUMERIC_INCLUDES  = ${NUMERIC_INCLUDES}")
    message (STATUS "NUMERIC_LIBRARIES = ${NUMERIC_LIBRARIES}")
  endif (NOT NUMERIC_FIND_QUIETLY)
else (HAVE_NUMERIC)
  if (NUMERIC_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NUMERIC!")
  endif (NUMERIC_FIND_REQUIRED)
endif (HAVE_NUMERIC)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NUMERIC_INCLUDES
  NUMERIC_LIBRARIES
  )
