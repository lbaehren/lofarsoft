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
  /opt/lib
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
## into account during the search. 

set (boost_libraries
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

foreach (lib ${boost_libraries})
  ## try to locate the library
  find_library (BOOST_${lib} ${lib}-gcc42-1_34_1 ${lib}-gcc-1_33_1 ${lib}-gcc ${lib}
    PATHS ${lib_locations}
    PATH_SUFFIXES boost-1_34_1 boost-1_33_1 boost
    )
  ## check if location was successful
  if (BOOST_${lib})
    list (APPEND BOOST_LIBRARIES ${BOOST_${lib}})
    set (continue_search 0)
  endif (BOOST_${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Check for symbols in the library
##
## We need this additional step especially for Python binding, as some of the
## required symbols might not be in place.

find_library (libboost_python boost_python-gcc-1_33_1 boost_python-gcc boost_python
  PATHS ${lib_locations}
  PATH_SUFFIXES boost-1_34_1 boost-1_33_1 boost
  )

if (libboost_python)
  foreach (boost_symbol PyMem_Malloc PyModule_Type PyMethod_Type)
    ## check for symbols in the library
    check_library_exists (${libboost_python}
      ${boost_symbol}
      ""
      BOOST_${boost_symbol}
      )
  endforeach (boost_symbol)
endif (libboost_python)

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
  endif (NOT Boost_FIND_QUIETLY)
else (HAVE_BOOST)
  if (Boost_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (Boost_FIND_REQUIRED)
endif (HAVE_BOOST)

## -----------------------------------------------------------------------------
