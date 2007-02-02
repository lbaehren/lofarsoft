# - Check for the presence of the Boost library
#
#  HAVE_BOOST      = do we have BOOST?
#  BOOST_INCLUDES  = location of the include files
#  BOOST_LIBRARIES = location of the libraries
#

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (BOOST_INCLUDES config.hpp
  PATHS /include /usr/include /usr/local/include /opt/include /sw/include
  PATH_SUFFIXES boost
  )

## -----------------------------------------------------------------------------
## Check for the various components of the library

set (libs
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
  find_library (BOOST_${lib} ${lib} ${lib}-gcc
    PATHS /lib /usr/lib /usr/local/lib /sw/lib
    PATH_SUFFIXES boost
    )
  ## check if location was successful
  if (BOOST_${lib})
    list (APPEND BOOST_LIBRARIES ${BOOST_${lib}})
  endif (BOOST_${lib})
endforeach (lib)

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
    message (STATUS "Boost library : ${BOOST_LIBRARIES}")
    message (STATUS "Boost headers : ${BOOST_INCLUDES}")
  endif (NOT Boost_FIND_QUIETLY)
else (HAVE_BOOST)
  if (Boost_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (Boost_FIND_REQUIRED)
endif (HAVE_BOOST)

## -----------------------------------------------------------------------------

mark_as_advanced (
  HAVE_BOOST
  BOOST_INCLUDES
  BOOST_LIBRARIES
)
