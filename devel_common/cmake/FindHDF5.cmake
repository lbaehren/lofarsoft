##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of HDF5
#
# The following variables are set when HDF5 is found:
#  HAVE_HDF5       = Set to true, if all components of HDF5 have been found.
#  HDF5_INCLUDES   = Include path for the header files of HDF5
#  HDF5_LIBRARIES  = Link these to use HDF5

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
## Determine library version


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

if (HAVE_HDF5)
  if (NOT HDF5_FIND_QUIETLY)
    message (STATUS "Found components for HDF5")
    message (STATUS "HDF5_INCLUDES  = ${HDF5_INCLUDES}")
    message (STATUS "HDF5_LIBRARIES = ${HDF5_LIBRARIES}")
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
  libhdf5
  libhdf5_hl
)
