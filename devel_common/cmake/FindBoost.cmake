##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of the Boost library
#
#  HAVE_BOOST      = do we have BOOST?
#  BOOST_INCLUDES  = location of the include files
#  BOOST_LIBRARIES = location of the libraries
#

include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckTypeSize)

set (include_locations
  /include
  /usr/include
  /usr/local/include
  /opt/include
  /sw/include
  )

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /sw/lib
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (BOOST_INCLUDES config.hpp
  PATHS ${include_locations}
  PATH_SUFFIXES
  boost-1_33_1
  boost
  )

## -----------------------------------------------------------------------------
## Check for the various components of the library

## The libraries tend to come in different name variants, so we need to take this
## into account during the search. The implementation below is not yet the most
## efficient solution; instead of listing all the variants explicitely, the more
## elegant solution would be to append the different suffixes at the
## "find_library()" command.

set (libs
  boost_date_time-gcc-1_33_1
  boost_filesystem-gcc-1_33_1
  boost_iostreams-gcc-1_33_1
  boost_program_options-gcc-1_33_1
  boost_python-gcc-1_33_1
  boost_regex-gcc-1_33_1
  boost_serialization-gcc-1_33_1
  boost_signals-gcc-1_33_1
  boost_test_exec_monitor-gcc-1_33_1
  boost_thread-gcc-1_33_1
  boost_unit_test_framework-gcc-1_33_1
  boost_wave-gcc-1_33_1
  ##
  boost_date_time-gcc
  boost_filesystem-gcc
  boost_iostreams-gcc
  boost_program_options-gcc
  boost_python-gcc
  boost_regex-gcc
  boost_serialization-gcc
  boost_signals-gcc
  boost_test_exec_monitor-gcc
  boost_thread-gcc
  boost_unit_test_framework-gcc
  boost_wave-gcc
  ## (potentially) older versions
  boost_date_time
  boost_filesystem
  boost_iostreams
  boost_program_options
  boost_python
  boost_regex
  boost_serialization
  boost_signals
  boost_test_exec_monitor
  boost_thread
  boost_unit_test_framework
  boost_wave
)

set (BOOST_LIBRARIES "")

foreach (lib ${libs})
  ## try to locate the library
  find_library (BOOST_${lib} ${lib}
    PATHS ${lib_locations}
    PATH_SUFFIXES boost-1_33_1 boost
    )
  ## check if location was successful
  if (BOOST_${lib})
    list (APPEND BOOST_LIBRARIES ${BOOST_${lib}})
  endif (BOOST_${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Check for symbols in the library
##
## We need this additional step especially for Python binding, as some of the
## required symbols might not be in place.

find_path (BOOST_LIBRARIES_DIR
  boost_python-gcc-mt-1_33_1 boost_python-gcc boost_python
    PATHS ${lib_locations}
    PATH_SUFFIXES boost-1_33_1 boost
)

CHECK_LIBRARY_EXISTS (boost_python PyMem_Malloc ${BOOST_LIBRARIES_DIR} BOOST_PyMem_Malloc)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BOOST_INCLUDES AND BOOST_LIBRARIES)
  SET (HAVE_BOOST TRUE)
ELSE (BOOST_INCLUDES AND BOOST_LIBRARIES)
  IF (NOT Boost_FIND_QUIETLY)
    IF (NOT BOOST_INCLUDES)
      MESSAGE (STATUS "Unable to find Boost header files!")
    ENDIF (NOT BOOST_INCLUDES)
    IF (NOT BOOST_LIBRARIES)
      MESSAGE (STATUS "Unable to find Boost library files!")
    ENDIF (NOT BOOST_LIBRARIES)
  ENDIF (NOT Boost_FIND_QUIETLY)
ENDIF (BOOST_INCLUDES AND BOOST_LIBRARIES)

if (HAVE_BOOST)
  if (NOT Boost_FIND_QUIETLY)
    message (STATUS "Found components for Boost")
    message (STATUS "Boost library ... : ${BOOST_LIBRARIES}")
    message (STATUS "Boost headers ... : ${BOOST_INCLUDES}")
    message (STATUS "Have PyMem_Malloc : ${BOOST_PyMem_Malloc}")
  endif (NOT Boost_FIND_QUIETLY)
else (HAVE_BOOST)
  if (Boost_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (Boost_FIND_REQUIRED)
endif (HAVE_BOOST)

## -----------------------------------------------------------------------------
