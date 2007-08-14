##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of Ruby
#
# The following variables are set when Ruby is found:
#  HAVE_Ruby       = Set to true, if all components of Ruby
#                          have been found.
#  Ruby_INCLUDES   = Include path for the header files of Ruby
#  Ruby_LIBRARIES  = Link these to use Ruby

## -----------------------------------------------------------------------------
## Search locations

set (bin_locations
  ../release/bin
  ../../release/bin
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt/casa/local/bin
)

set (include_locations
  ../release/include
  ../../release/include
  /usr/include
  /usr/local/include
  /sw/include
  /opt/casa/local/include
)

set (lib_locations
  ../release/lib
  ../../release/lib
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  /opt/casa/local/lib
)

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
  PATHS ${include_locations}
  PATH_SUFFIXES ruby/1.8/${ARCH} ruby/1.8/universal-darwin8.0
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (Ruby_LIBRARIES ruby
  PATHS ${lib_locations}
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

