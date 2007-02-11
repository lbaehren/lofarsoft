## -----------------------------------------------------------------------------
# - Check for the presence of G2C
#
# The following variables are set when G2C is found:
#  HAVE_G2C       = Set to true, if all components of G2C
#                          have been found.
#  G2C_INCLUDES   = Include path for the header files of G2C
#  G2C_LIBRARIES  = Link these to use G2C
## -----------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Check for the header files

find_path (G2C_INCLUDES g2c.h
  PATHS
  /usr/local/include
  /usr/include
  /sw/include
  PATH_SUFFIXES
  gcc
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (G2C_LIBRARIES g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (G2C_INCLUDES AND G2C_LIBRARIES)
  set (HAVE_G2C TRUE)
else (G2C_INCLUDES AND G2C_LIBRARIES)
  if (NOT G2C_FIND_QUIETLY)
    if (NOT G2C_INCLUDES)
      message (STATUS "Unable to find G2C header files!")
    endif (NOT G2C_INCLUDES)
    if (NOT G2C_LIBRARIES)
      message (STATUS "Unable to find G2C library files!")
    endif (NOT G2C_LIBRARIES)
  endif (NOT G2C_FIND_QUIETLY)
endif (G2C_INCLUDES AND G2C_LIBRARIES)

if (HAVE_G2C)
  if (NOT G2C_FIND_QUIETLY)
    message (STATUS "Found components for G2C")
    message (STATUS "G2C_INCLUDES  = ${G2C_INCLUDES}")
    message (STATUS "G2C_LIBRARIES = ${G2C_LIBRARIES}")
  endif (NOT G2C_FIND_QUIETLY)
else (HAVE_G2C)
  if (G2C_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find G2C!")
  endif (G2C_FIND_REQUIRED)
endif (HAVE_G2C)

mark_as_advanced (
  HAVE_G2C
  G2C_LIBRARIES
  G2C_INCLUDES
  )
