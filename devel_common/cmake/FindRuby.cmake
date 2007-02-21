# - Check for the presence of Ruby
#
# The following variables are set when Ruby is found:
#  HAVE_Ruby       = Set to true, if all components of Ruby
#                          have been found.
#  Ruby_INCLUDES   = Include path for the header files of Ruby
#  Ruby_LIBRARIES  = Link these to use Ruby

## -----------------------------------------------------------------------------
## System properties; library and header files may reside in an architecture-
## dependent subdirectory

if (UNIX)
  if (APPLE)
    if (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      set (ARCH "powerpc-darwin")
    else (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      set (ARCH "intel-darwin")
    endif (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
  else (APPLE)
    message (STATUS "General UNIX system detected.")
  endif (APPLE)
else (UNIX)
  message (ERROR "No UNIX-like system!")
endif (UNIX)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (Ruby_INCLUDES ruby.h
  PATHS
  /usr/local/include /usr/include /sw/include
  /usr/lib /sw/lib
  PATH_SUFFIXES ruby/1.8/${ARCH} ruby/1.8/universal-darwin8.0
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (Ruby_LIBRARIES ruby
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  PATH_SUFFIXES ruby/1.8 ruby/1.9
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (Ruby_INCLUDES AND Ruby_LIBRARIES)
  set (HAVE_Ruby TRUE)
else (Ruby_INCLUDES AND Ruby_LIBRARIES)
  if (NOT Ruby_FIND_QUIETLY)
    if (NOT Ruby_INCLUDES)
      message (STATUS "Unable to find Ruby header files!")
    endif (NOT Ruby_INCLUDES)
    if (NOT Ruby_LIBRARIES)
      message (STATUS "Unable to find Ruby library files!")
    endif (NOT Ruby_LIBRARIES)
  endif (NOT Ruby_FIND_QUIETLY)
endif (Ruby_INCLUDES AND Ruby_LIBRARIES)

if (HAVE_Ruby)
  if (NOT Ruby_FIND_QUIETLY)
    message (STATUS "Found components for Ruby")
    message (STATUS "Ruby_INCLUDES  = ${Ruby_INCLUDES}")
    message (STATUS "Ruby_LIBRARIES = ${Ruby_LIBRARIES}")
  endif (NOT Ruby_FIND_QUIETLY)
else (HAVE_Ruby)
  if (Ruby_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Ruby!")
  endif (Ruby_FIND_REQUIRED)
endif (HAVE_Ruby)

mark_as_advanced (
  HAVE_Ruby
  Ruby_LIBRARIES
  Ruby_INCLUDES
  )
