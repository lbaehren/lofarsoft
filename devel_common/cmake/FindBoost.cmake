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

# - Check for the presence of the Boost library
#
#  HAVE_BOOST      = do we have BOOST?
#  BOOST_INCLUDES  = location of the include files
#  BOOST_LIBRARIES = location of the libraries
#

include (CheckIncludeFiles)
include (CheckTypeSize)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files and the various module libraries

set (BOOST_FIND_python_ONLY NO)

set (BOOST_FIND_date_time           YES )
set (BOOST_FIND_filesystem          YES )
set (BOOST_FIND_iostreams           YES )
set (BOOST_FIND_program_options     YES )
set (BOOST_FIND_python              YES )
set (BOOST_FIND_regex               YES )
set (BOOST_FIND_serialization       YES )
set (BOOST_FIND_signals             YES )
set (BOOST_FIND_system              YES )
set (BOOST_FIND_thread              YES )
set (BOOST_FIND_unit_test_framework YES )
set (BOOST_FIND_wave                YES )

if (BOOST_FIND_python_ONLY)
  
  if (NOT BOOST_FIND_QUIETLY)
    message (STATUS "[FindBoost] Configuration for boost_python only.")
  endif (NOT BOOST_FIND_QUIETLY)

  set (BOOST_FIND_python YES)

  set (BOOST_FIND_date_time           NO )
  set (BOOST_FIND_filesystem          NO )
  set (BOOST_FIND_iostreams           NO )
  set (BOOST_FIND_program_options     NO )
  set (BOOST_FIND_regex               NO )
  set (BOOST_FIND_serialization       NO )
  set (BOOST_FIND_signals             NO )
  set (BOOST_FIND_system              NO )
  set (BOOST_FIND_thread              NO )
  set (BOOST_FIND_unit_test_framework NO )
  set (BOOST_FIND_wave                NO )

endif (BOOST_FIND_python_ONLY)

if (BOOST_FIND_date_time)
  list (APPEND boost_libraries boost_date_time)
endif (BOOST_FIND_date_time)

if (BOOST_FIND_filesystem)
  list (APPEND boost_libraries boost_filesystem)
endif (BOOST_FIND_filesystem)

if (BOOST_FIND_iostreams)
  list (APPEND boost_libraries boost_iostreams)
endif (BOOST_FIND_iostreams)

if (BOOST_FIND_program_options)
  list (APPEND boost_libraries boost_program_options)
endif (BOOST_FIND_program_options)

if (BOOST_FIND_python)
  list (APPEND boost_libraries boost_python)
endif (BOOST_FIND_python)

if (BOOST_FIND_regex)
  list (APPEND boost_libraries boost_regex)
endif (BOOST_FIND_regex)

if (BOOST_FIND_serialization)
  list (APPEND boost_libraries boost_serialization)
endif (BOOST_FIND_serialization)

if (BOOST_FIND_signals)
  list (APPEND boost_libraries boost_signals)
endif (BOOST_FIND_signals)

if (BOOST_FIND_system)
  list (APPEND boost_libraries boost_system)
endif (BOOST_FIND_system)

if (BOOST_FIND_thread)
  list (APPEND boost_libraries boost_thread)
endif (BOOST_FIND_thread)

if (BOOST_FIND_unit_test_framework)
  list (APPEND boost_libraries boost_unit_test_framework)
endif (BOOST_FIND_unit_test_framework)

if (BOOST_FIND_wave)
  list (APPEND boost_libraries boost_wave)
endif (BOOST_FIND_wave)

## initialize list of detected libraries

set (BOOST_LIBRARIES "")

foreach (boost_version 1_34_1 1_33_1)

  ## Check for the header files ------------------

  ## <boost/config.hpp>
  find_path (BOOST_INCLUDES_tmp boost/config.hpp boost/python.hpp
    PATHS ${include_locations}
    PATH_SUFFIXES
    boost-${boost_version}
    boost
    .
    NO_DEFAULT_PATH
    )

  if (BOOST_INCLUDES_tmp)
    get_filename_component (BOOST_INCLUDES ${BOOST_INCLUDES_tmp} ABSOLUTE)
  endif (BOOST_INCLUDES_tmp)

  ## Check for the module libraries --------------
  
  foreach (lib ${boost_libraries})
    ## try to locate the library
    find_library (BOOST_${lib} ${lib} ${lib}-gcc42-${boost_version} ${lib}-mt-${boost_version} ${lib}-gcc
      PATHS ${lib_locations}
      PATH_SUFFIXES boost boost-${boost_version}
      NO_DEFAULT_PATH
      )
    ## check if location was successful
    if (BOOST_${lib})
      list (APPEND BOOST_LIBRARIES ${BOOST_${lib}})
      set (continue_search 0)
    endif (BOOST_${lib})
  endforeach (lib)
  
endforeach (boost_version)

## -----------------------------------------------------------------------------
## Check for symbols in the library
##
## We need this additional step especially for Python binding, as some of the
## required symbols might not be in place.
##
## _NOTE_ This does not yet properly as 
##        "nm libboost_python-mt-1_34_1.dylib | grep _PyMem_Malloc"
##        report the symbol to be present in the library.

if (BOOST_boost_python)

  ## load CMake module required for checking symbols within a library
  include (CheckLibraryExists)

  check_library_exists (${BOOST_boost_python} PyMem_Malloc "" BOOST__PyMem_Malloc)
  check_library_exists (${BOOST_boost_python} PyModule_Type "" BOOST__PyModule_Type)
  check_library_exists (${BOOST_boost_python} PyMethod_Type "" BOOST__PyMethod_Type)
  check_library_exists (${BOOST_boost_python} PyErr_WarnEx "" BOOST__PyErr_WarnEx)

endif (BOOST_boost_python)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BOOST_INCLUDES AND BOOST_LIBRARIES)
  SET (HAVE_BOOST TRUE)
ELSE (BOOST_INCLUDES AND BOOST_LIBRARIES)
  IF (NOT BOOST_FIND_QUIETLY)
    IF (NOT BOOST_INCLUDES)
      MESSAGE (STATUS "Unable to find Boost header files!")
    ENDIF (NOT BOOST_INCLUDES)
    IF (NOT BOOST_LIBRARIES)
      MESSAGE (STATUS "Unable to find Boost library files!")
    ENDIF (NOT BOOST_LIBRARIES)
  ENDIF (NOT BOOST_FIND_QUIETLY)
ENDIF (BOOST_INCLUDES AND BOOST_LIBRARIES)

if (HAVE_BOOST)
  if (NOT BOOST_FIND_QUIETLY)
    message (STATUS "Found components for Boost")
    ## list the components for which the search was enabled
    message (STATUS "date_time ......... : ${BOOST_FIND_date_time}")
    message (STATUS "filesystem ........ : ${BOOST_FIND_filesystem}")
    message (STATUS "iostreams ......... : ${BOOST_FIND_iostreams}")
    message (STATUS "program_options ... : ${BOOST_FIND_program_options}")
    message (STATUS "python ............ : ${BOOST_FIND_python}")
    message (STATUS "regex ............. : ${BOOST_FIND_regex}")
    message (STATUS "signals ........... : ${BOOST_FIND_signals}")
    message (STATUS "system ............ : ${BOOST_FIND_system}")
    message (STATUS "unit_test_framework : ${BOOST_FIND_unit_test_framework}")
    message (STATUS "thread ............ : ${BOOST_FIND_thread}")
    message (STATUS "wave .............. : ${BOOST_FIND_wave}")
    ## show search results
    message (STATUS "Boost library ..... : ${BOOST_LIBRARIES}")
    message (STATUS "Boost headers ..... : ${BOOST_INCLUDES}")
  endif (NOT BOOST_FIND_QUIETLY)
else (HAVE_BOOST)
  if (BOOST_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (BOOST_FIND_REQUIRED)
endif (HAVE_BOOST)

## -----------------------------------------------------------------------------

mark_as_advanced (
  BOOST_INCLUDES
  BOOST_LIBRARIES
)
