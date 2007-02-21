# - Check for the presence of Globus
#
# The following variables are set when Globus is found:
#  HAVE_Globus       = Set to true, if all components of Globus
#                          have been found.
#  Globus_INCLUDES   = Include path for the header files of Globus
#  Globus_LIBRARIES  = Link these to use Globus

## -----------------------------------------------------------------------------
## Check for the header files

find_path (Globus_INCLUDES globus.h
  PATHS /usr/local/include /usr/include /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (Globus_LIBRARIES globus
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (Globus_INCLUDES AND Globus_LIBRARIES)
  set (HAVE_Globus TRUE)
else (Globus_INCLUDES AND Globus_LIBRARIES)
  if (NOT Globus_FIND_QUIETLY)
    if (NOT Globus_INCLUDES)
      message (STATUS "Unable to find Globus header files!")
    endif (NOT Globus_INCLUDES)
    if (NOT Globus_LIBRARIES)
      message (STATUS "Unable to find Globus library files!")
    endif (NOT Globus_LIBRARIES)
  endif (NOT Globus_FIND_QUIETLY)
endif (Globus_INCLUDES AND Globus_LIBRARIES)

if (HAVE_Globus)
  if (NOT Globus_FIND_QUIETLY)
    message (STATUS "Found components for Globus")
    message (STATUS "Globus_INCLUDES  = ${Globus_INCLUDES}")
    message (STATUS "Globus_LIBRARIES = ${Globus_LIBRARIES}")
  endif (NOT Globus_FIND_QUIETLY)
else (HAVE_Globus)
  if (Globus_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Globus!")
  endif (Globus_FIND_REQUIRED)
endif (HAVE_Globus)

mark_as_advanced (
  HAVE_Globus
  Globus_LIBRARIES
  Globus_INCLUDES
  )
