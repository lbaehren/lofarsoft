# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

# - Check for the presence of HDF5
#
# The following variables are set when HDF5 is found:
#  HAVE_HDF5            = Set to true, if all components of HDF5 have been found.
#  HDF5_INCLUDES        = Include path for the header files of HDF5
#  HDF5_LIBRARIES       = Link these to use HDF5
#  HDF5_MAJOR_VERSION   = Major version of the HDF5 library
#  HDF5_MINOR_VERSION   = Minor version of the HDF5 library
#  HDF5_RELEASE_VERSION = Release version of the HDF5 library

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (HDF5_INCLUDES hdf5.h H5LT.h
  PATHS ${include_locations}
  PATH_SUFFIXES hdf5
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library components

## [1] Core library

find_library (libhdf5
  NAMES hdf5
  PATHS ${lib_locations}
  PATH_SUFFIXES hdf5
  NO_DEFAULT_PATH
)

if (libhdf5)
  set (HDF5_LIBRARIES ${libhdf5})
endif (libhdf5)

## [2] Additional libraries

FIND_LIBRARY (libhdf5_hl
  NAMES hdf5_hl
  PATHS ${lib_locations}
  PATH_SUFFIXES hdf5
  NO_DEFAULT_PATH
)

if (libhdf5_hl)
  list (APPEND HDF5_LIBRARIES ${libhdf5_hl})
endif (libhdf5_hl)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (HDF5_INCLUDES AND HDF5_LIBRARIES)
  set (HAVE_HDF5 TRUE)
else (HDF5_INCLUDES AND HDF5_LIBRARIES)
  set (HAVE_HDF5 FALSE)
  if (NOT HDF5_FIND_QUIETLY)
    if (NOT HDF5_INCLUDES)
      message (STATUS "Unable to find HDF5 header files!")
    endif (NOT HDF5_INCLUDES)
    if (NOT HDF5_LIBRARIES)
      message (STATUS "Unable to find HDF5 library files!")
    endif (NOT HDF5_LIBRARIES)
  endif (NOT HDF5_FIND_QUIETLY)
endif (HDF5_INCLUDES AND HDF5_LIBRARIES)

## -----------------------------------------------------------------------------
## Determine library version

if (HAVE_HDF5)

  list (APPEND CMAKE_REQUIRED_LIBRARIES ${HDF5_LIBRARIES})
  
  try_run (var_exit var_compiled
    ${CMAKE_BINARY_DIR}
    ${CMAKE_MODULE_PATH}/TestHDF5Version.cc
    COMPILE_DEFINITIONS -I${HDF5_INCLUDES}
    CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}
    COMPILE_OUTPUT_VARIABLE var_compile
    RUN_OUTPUT_VARIABLE var_run
    )

  ## process the output of the test program

  string (REGEX MATCH "maj=.*:min" HDF5_MAJOR_VERSION ${var_run})
  string (REGEX REPLACE "maj=" "" HDF5_MAJOR_VERSION ${HDF5_MAJOR_VERSION})
  string (REGEX REPLACE ":min" "" HDF5_MAJOR_VERSION ${HDF5_MAJOR_VERSION})
  
  string (REGEX MATCH "min=.*:rel" HDF5_MINOR_VERSION ${var_run})
  string (REGEX REPLACE "min=" "" HDF5_MINOR_VERSION ${HDF5_MINOR_VERSION})
  string (REGEX REPLACE ":rel" "" HDF5_MINOR_VERSION ${HDF5_MINOR_VERSION})

  string (REGEX MATCH "rel=.*:" HDF5_RELEASE_VERSION ${var_run})
  string (REGEX REPLACE "rel=" "" HDF5_RELEASE_VERSION ${HDF5_RELEASE_VERSION})
  string (REGEX REPLACE ":" "" HDF5_RELEASE_VERSION ${HDF5_RELEASE_VERSION})
  
endif (HAVE_HDF5)

## -----------------------------------------------------------------------------
## Feedback

if (HAVE_HDF5)
  if (NOT HDF5_FIND_QUIETLY)
    message (STATUS "Found components for HDF5")
    message (STATUS "HDF5_INCLUDES        = ${HDF5_INCLUDES}")
    message (STATUS "HDF5_LIBRARIES       = ${HDF5_LIBRARIES}")
    message (STATUS "HDF5_MAJOR_VERSION   = ${HDF5_MAJOR_VERSION}")
    message (STATUS "HDF5_MINOR_VERSION   = ${HDF5_MINOR_VERSION}")
    message (STATUS "HDF5_RELEASE_VERSION = ${HDF5_RELEASE_VERSION}")
  endif (NOT HDF5_FIND_QUIETLY)
else (HAVE_HDF5)
  if (HDF5_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find HDF5!")
  endif (HDF5_FIND_REQUIRED)
endif (HAVE_HDF5)

## -----------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  HDF5_INCLUDES
  HDF5_LIBRARIES
  HDF5_MAJOR_VERSION
  HDF5_MINOR_VERSION
  libhdf5
  libhdf5_hl
)
