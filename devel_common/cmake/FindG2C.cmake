##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
# - Check for the presence of G2C
#
# The following variables are set when G2C is found:
#  HAVE_G2C       = Set to true, if all components of G2C have been found.
#  G2C_INCLUDES   = Include path for the header files of G2C
#  G2C_LIBRARIES  = Link these to use G2C
## -----------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (G2C_INCLUDES g2c.h f2c.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  gcc
  gcc-lib/i486-linux/3.3.5/include
  gcc-lib/i586-suse-linux/3.3.3/include
  gcc-lib/i586-suse-linux/3.3.4/include
  gcc-lib/i586-suse-linux/3.3.5/include
  gcc/x86_64-redhat-linux/3.4.3/include
  )

## -----------------------------------------------------------------------------
## Check for the library

if (UNIX)
  if (APPLE)
    IF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      list (APPEND lib_locations
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.0
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.1
	)
    ELSE (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      list (APPEND lib_locations
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin8/4.0.0
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin8/4.0.1
	)
    ENDIF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
  else (APPLE)
    list (APPEND lib_locations
      /usr/lib/gcc/x86_64-redhat-linux/3.4.3/32
      /usr/lib/gcc/x86_64-redhat-linux/3.4.3
      )
  endif (APPLE)
endif (UNIX)

find_library (G2C_LIBRARIES
  NAMES gfortran g2c gcc
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (G2C_INCLUDES AND G2C_LIBRARIES)
  set (HAVE_G2C TRUE)
else (G2C_INCLUDES AND G2C_LIBRARIES)
  set (HAVE_G2C FALSE)
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

## -----------------------------------------------------------------------------
## Mark as advanced ...

MARK_AS_ADVANCED (
  G2C_INCLUDES
  G2C_LIBRARIES
  )