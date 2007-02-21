# - Check for the presence of GSL
#
# The following variables are set when GSL is found:
#  HAVE_GSL       = Set to true, if all components of GSL have been found.
#  GSL_INCLUDES   = Include path for the header files of GSL
#  GSL_LIBRARIES  = Link these to use GSL

## -----------------------------------------------------------------------------
## Check for existance of GSL configuration utility; if it is present, we can
## use it to obtain further information on the GSL installation.

find_program (GSL_CONFIG gsl-config
  PATHS /usr/bin /usr/local/bin /sw/bin
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (GSL_INCLUDES gsl_version.h
  PATHS /usr/include /usr/local/include /sw/include
  PATH_SUFFIXES gsl
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library; if we have "gsl-config" we can ask it for the linker
## parameters

if (GSL_CONFIG)
  execute_process (
    COMMAND ${GSL_CONFIG} --libs
    OUTPUT_VARIABLE GSL_LIBRARIES)  
else (GSL_CONFIG)
  find_library (GSL_LIBRARIES gsl
    PATHS /usr/lib /usr/local/lib /sw/lib
    NO_DEFAULT_PATH
    )
endif (GSL_CONFIG)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GSL_INCLUDES AND GSL_LIBRARIES)
  set (HAVE_GSL TRUE)
else (GSL_INCLUDES AND GSL_LIBRARIES)
  if (NOT GSL_FIND_QUIETLY)
    if (NOT GSL_INCLUDES)
      message (STATUS "Unable to find GSL header files!")
    endif (NOT GSL_INCLUDES)
    if (NOT GSL_LIBRARIES)
      message (STATUS "Unable to find GSL library files!")
    endif (NOT GSL_LIBRARIES)
  endif (NOT GSL_FIND_QUIETLY)
endif (GSL_INCLUDES AND GSL_LIBRARIES)

if (HAVE_GSL)
  if (NOT GSL_FIND_QUIETLY)
    message (STATUS "Found components for GSL")
    message (STATUS "GSL_INCLUDES  = ${GSL_INCLUDES}")
    message (STATUS "GSL_LIBRARIES = ${GSL_LIBRARIES}")
  endif (NOT GSL_FIND_QUIETLY)
else (HAVE_GSL)
  if (GSL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GSL!")
  endif (GSL_FIND_REQUIRED)
endif (HAVE_GSL)

mark_as_advanced (
  HAVE_GSL
  GSL_LIBRARIES
  GSL_INCLUDES
  )
