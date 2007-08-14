##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of SWIG
#
# The following variables are set when SWIG is found:
#  HAVE_SWIG       = Set to true, if all components of SWIG
#                          have been found.
#  SWIG_INCLUDES   = Include path for the header files of SWIG
#  SWIG_LIBRARIES  = Link these to use SWIG
#  SWIG_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt/casa/local/bin
  ../release/bin
  ../../release/bin
)

set (include_locations
  /usr/include
  /usr/local/include
  /sw/include
  /opt/casa/local/include
  ../release/include
  ../../release/include
)

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt/casa/local/lib
  ../release/lib
  ../../release/lib
)

## -----------------------------------------------------------------------------
## Find executable

find_program (SWIG_BIN swig
  PATHS ${bin_locations}
  )

## -----------------------------------------------------------------------------
## Check for the header files

#find_path (SWIG_INCLUDES <header file(s)>
#  PATHS ${include_locations}
#  PATH_SUFFIXES <optional path extension>
#  NO_DEFAULT_PATH
#  )

## -----------------------------------------------------------------------------
## Check for the library

#find_library (SWIG_LIBRARIES <package name>
#  PATHS ${lib_locations}
#  NO_DEFAULT_PATH
#  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (SWIG_BIN)
  set (HAVE_SWIG TRUE)
endif (SWIG_BIN)

#if (SWIG_INCLUDES AND SWIG_LIBRARIES)
#  set (HAVE_SWIG TRUE)
#else (SWIG_INCLUDES AND SWIG_LIBRARIES)
#  if (NOT SWIG_FIND_QUIETLY)
#    if (NOT SWIG_INCLUDES)
#      message (STATUS "Unable to find SWIG header files!")
#    endif (NOT SWIG_INCLUDES)
#    if (NOT SWIG_LIBRARIES)
#      message (STATUS "Unable to find SWIG library files!")
#    endif (NOT SWIG_LIBRARIES)
#  endif (NOT SWIG_FIND_QUIETLY)
#endif (SWIG_INCLUDES AND SWIG_LIBRARIES)

if (HAVE_SWIG)
  if (NOT SWIG_FIND_QUIETLY)
    message (STATUS "Found components for SWIG")
    message (STATUS "SWIG_BIN         = ${SWIG_BIN}")
#    message (STATUS "SWIG_INCLUDES  = ${SWIG_INCLUDES}")
#    message (STATUS "SWIG_LIBRARIES = ${SWIG_LIBRARIES}")
  endif (NOT SWIG_FIND_QUIETLY)
else (HAVE_SWIG)
  if (SWIG_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find SWIG!")
  endif (SWIG_FIND_REQUIRED)
endif (HAVE_SWIG)

## -----------------------------------------------------------------------------
## Mark advanced variables

#mark_as_advanced (
#  )
