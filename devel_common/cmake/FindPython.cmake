##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 696 2007-09-06 15:00:34Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of <PACKAGE>
#
# The following variables are set when PYTHON is found:
#  HAVE_PYTHON       = Set to true, if all components of PYTHON
#                          have been found.
#  PYTHON_EXECUTABLE = Location of the Python executable
#  PYTHON_INCLUDES   = Include path for the header files of PYTHON
#  PYTHON_LIBRARIES  = Link these to use PYTHON
#  PYTHON_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/bin
  /usr/local/bin
  /usr/X11R6/bin
  /opt/bin
  /opt/local/bin
  /sw/bin
  )

set (lib_locations
  /usr/lib
  /usr/local/lib
  /usr/X11R6/lib
  /opt/lib
  /opt/local/lib
  /sw/lib
  )

set (include_locations
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  )

## -----------------------------------------------------------------------------

foreach (python_version 2.6 2.5 2.4 2.3)

  ## Check for executable
  
  find_program (PYTHON_EXECUTABLE python${python_version}
    PATHS ${bin_locations}
    PATH_SUFFIXES python${python_version}
    NO_DEFAULT_PATH
    )
  
  ## Check for the header files

  find_path (PYTHON_INCLUDES Python.h
    PATHS ${include_locations}
    PATH_SUFFIXES python${python_version}
    NO_DEFAULT_PATH
    )

  ## Check for the library

  find_library (PYTHON_LIBRARIES python${python_version}
    PATHS ${lib_locations}
    PATH_SUFFIXES python${python_version}/config
    NO_DEFAULT_PATH
    )

  # check if components have been found
  if (NOT PYTHON_INCLUDES OR NOT PYTHON_LIBRARIES)
    if (NOT PYTHON_FIND_QUIETLY)
      message (SEND_ERROR "No consistent set of files found for Python ${python_version}")
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
