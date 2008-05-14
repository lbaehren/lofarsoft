##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of ROOT
#
# The following variables are set when ROOT is found:
#  HAVE_ROOT       = Set to true, if all components of ROOT have been found.
#  ROOT_INCLUDES   = Include path for the header files of ROOT
#  ROOT_LIBRARIES  = Link these to use ROOT
#  ROOT_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (ROOT_INCLUDES tCanvas.h TCint.h TObject.h
  PATHS
  ${include_locations}
  /opt/root/include
  /opt/auger/root-v5.18.00
  $ENV{ROOTSYS}
  PATH_SUFFIXES
  root
  root/include
  include
  NO_DEFAULT_PATH
  )

get_filename_component (ROOT_INCLUDES ${ROOT_INCLUDES} ABSOLUTE)

## -----------------------------------------------------------------------------
## Check for the libraries

set (libs
  ## output from 'root-config --glibs'
  Core
  Cint
  RIO
  Net
  Hist
  Graf
  Graf3d
  Gpad
  Tree
  Rint
  Postscript
  Matrix
  Physics
  MathCore
  freetype
  Gui
  pthread
  )

foreach (lib ${libs})
  ## try to locate the library
  find_library (root${lib} ${lib}
    PATHS
    ${lib_locations}
    /opt/root/lib
    /opt/auger/root-v5.18.00
    $ENV{ROOTSYS}
    PATH_SUFFIXES
    root
    root/lib
    lib
    NO_DEFAULT_PATH
    )
  ## if the library could be located, it is added to the list
  if (root${lib})
    list (APPEND ROOT_LIBRARIES ${root${lib}})
  endif (root${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (ROOT_INCLUDES AND ROOT_LIBRARIES)
  set (HAVE_ROOT TRUE)
else (ROOT_INCLUDES AND ROOT_LIBRARIES)
  set (HAVE_ROOT FALSE)
  if (NOT ROOT_FIND_QUIETLY)
    if (NOT ROOT_INCLUDES)
      message (STATUS "Unable to find ROOT header files!")
    endif (NOT ROOT_INCLUDES)
    if (NOT ROOT_LIBRARIES)
      message (STATUS "Unable to find ROOT library files!")
    endif (NOT ROOT_LIBRARIES)
  endif (NOT ROOT_FIND_QUIETLY)
endif (ROOT_INCLUDES AND ROOT_LIBRARIES)

if (HAVE_ROOT)
  if (NOT ROOT_FIND_QUIETLY)
    message (STATUS "Found components for ROOT")
    message (STATUS "ROOT_INCLUDES  = ${ROOT_INCLUDES}")
    message (STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")
  endif (NOT ROOT_FIND_QUIETLY)
else (HAVE_ROOT)
  if (ROOT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find ROOT!")
  endif (ROOT_FIND_REQUIRED)
endif (HAVE_ROOT)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  ROOT_INCLUDES
  ROOT_LIBRARIES
  )

foreach (lib ${libs})
    if (root${lib})
      mark_as_advanced (root${lib})
  endif (root${lib})
endforeach (lib)
