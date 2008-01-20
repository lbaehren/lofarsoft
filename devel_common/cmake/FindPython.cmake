##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 696 2007-09-06 15:00:34Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of <PACKAGE>
#
# The following variables are set when PYTHON is found:
#
#  HAVE_PYTHON       = Set to true, if all components of PYTHON
#                          have been found.
#  PYTHON_EXECUTABLE = Location of the Python executable
#  PYTHON_INCLUDES   = Include path for the header files of PYTHON
#  PYTHON_LIBRARIES  = Link these to use PYTHON
#  PYTHON_LFGLAS     = Linker flags (optional)
#
# Beside the core components of Python we also include a search for optional
# packages which might be installed as well:
#
#  NUMPY_INCLUDES    = Include path for the header files of NUMPY package
#  NUM_UTIL_INCLUDES = Include path for the header files of NUM_UTIL package
#

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  ## local installation
  ../release/bin
  ../../release/bin
  ../../../release/bin
  ## system-wide installation
  /opt/bin
  /opt/local/bin
  /sw/bin
  /dp/bin
  /usr/bin
  /usr/local/bin
  /usr/X11R6/bin
  )

set (lib_locations
  ## local installation
  ../release/lib
  ../../release/lib
  ../../../release/lib
  ## system-wide installation
  /opt/lib
  /opt/local/lib
  /sw/lib
  /dp/lib
  /usr/lib
  /usr/local/lib
  /usr/X11R6/lib
  )

set (include_locations
  ## local installation
  ../release/include
  ../../release/include
  ../../../release/include
  ## system-wide installation
  /opt/include
  /opt/local/include
  /sw/include
  /dp/include
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  )

## -----------------------------------------------------------------------------

foreach (python_version 2.6 2.5 2.4 2.3)

  ## Check for executable
  
  find_program (PYTHON_EXECUTABLE python${python_version}
    PATHS ${bin_locations}
    NO_DEFAULT_PATH
    )
  
  ## Check for the PYTHON header files

  find_path (PYTHON_INCLUDES Python.h
    PATHS ${include_locations}
    PATH_SUFFIXES python${python_version}
    NO_DEFAULT_PATH
    )

  ## Check for the NUMPY header files

  find_path (NUMPY_INCLUDES numpy/arrayobject.h
    PATHS ${lib_locations} ${include_locations}
    PATH_SUFFIXES
    python
    python${python_version}
    python${python_version}/site-packages/numpy/core/include
    NO_DEFAULT_PATH
    )

  ## Check for the NUM_UTIL header files and libraries

  find_path (NUM_UTIL_INCLUDES num_util/num_util.h
    PATHS ${include_locations}
    PATH_SUFFIXES
    python
    python${python_version}
    NO_DEFAULT_PATH
    )

  find_library (NUM_UTIL_LIBRARIES num_util
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )

  ## Check for the library

  find_library (PYTHON_LIBRARIES python${python_version}
    PATHS ${lib_locations}
    PATH_SUFFIXES . python${python_version}/config
    NO_DEFAULT_PATH
    )

  # check if components have been found
  if (NOT PYTHON_INCLUDES OR NOT PYTHON_LIBRARIES)
    if (NOT PYTHON_FIND_QUIETLY)
      message (STATUS "No consistent set of files found for Python ${python_version}")
    endif (NOT PYTHON_FIND_QUIETLY)
  endif (NOT PYTHON_INCLUDES OR NOT PYTHON_LIBRARIES)
endforeach (python_version)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PYTHON_INCLUDES AND PYTHON_LIBRARIES)
  set (HAVE_PYTHON TRUE)
else (PYTHON_INCLUDES AND PYTHON_LIBRARIES)
  set (HAVE_PYTHON FALSE)
  if (NOT PYTHON_FIND_QUIETLY)
    if (NOT PYTHON_INCLUDES)
      message (STATUS "Unable to find PYTHON header files!")
    endif (NOT PYTHON_INCLUDES)
    if (NOT PYTHON_LIBRARIES)
      message (STATUS "Unable to find PYTHON library files!")
    endif (NOT PYTHON_LIBRARIES)
  endif (NOT PYTHON_FIND_QUIETLY)
endif (PYTHON_INCLUDES AND PYTHON_LIBRARIES)

if (HAVE_PYTHON)
  if (NOT PYTHON_FIND_QUIETLY)
    message (STATUS "Found components for PYTHON")
    message (STATUS "PYTHON_EXECUTABLE  = ${PYTHON_EXECUTABLE}")
    message (STATUS "PYTHON_INCLUDES    = ${PYTHON_INCLUDES}")
    message (STATUS "PYTHON_LIBRARIES   = ${PYTHON_LIBRARIES}")
    message (STATUS "NUMPY_INCLUDES     = ${NUMPY_INCLUDES}")
  endif (NOT PYTHON_FIND_QUIETLY)
else (HAVE_PYTHON)
  if (PYTHON_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PYTHON!")
  endif (PYTHON_FIND_REQUIRED)
endif (HAVE_PYTHON)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PYTHON_INCLUDES
  PYTHON_LIBRARIES
  )
