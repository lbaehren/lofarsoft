# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
# |   Lars B"ahren <bahren@astron.nl>                                           |
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

# - Check for the presence of CASAPY
#
# The following variables are set when CASAPY is found:
#  HAVE_CASAPY       = Set to true, if all components of CASAPY have been found.
#  CASAPY_INCLUDES   = Include path for the header files of CASAPY
#  CASAPY_LIBRARIES  = Link these to use CASAPY
#  CASAPY_LFLAGS     = Linker flags (optional)

if (NOT FIND_CASAPY_CMAKE)
  
  set (FIND_CASAPY_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)

  ##_____________________________________________________________________________
  ## Base directory of CASApy installation
  
  if (APPLE)
    find_path (CASAPY_BASEDIR Contents/Info.plist Contents/PkgInfo
      PATHS
      /Applications/CASA.app
      )
  else (APPLE)
    message (WARNING "FindCASApy only implemented for Mac OS X yet!")
  endif (APPLE)
  
  if (CASAPY_BASEDIR)
    set (CASAPY_FRAMEWORKS "${CASAPY_BASEDIR}/Contents/Frameworks")
  else (CASAPY_BASEDIR)
    message (WARNING "Unable to locate CASApy installation base!")
  endif (CASAPY_BASEDIR)
  
  ##_____________________________________________________________________________
  ##                                                                        Boost
  
  set (CASAPY_BOOST_VERSION "1_35")
  
  find_path (CASAPY_BOOST_INCLUDES boost.h
    PATHS ${include_locations}
    PATH_SUFFIXES casa
    NO_DEFAULT_PATH
    )
  
  set (CASAPY_BOOST_LIBRARIES)

  foreach (_libboost 
      boost_filesystem
      boost_program_options
      boost_python
      boost_regex
      boost_system
      )
    ## convert name to upper case
    string (TOUPPER ${_libboost} _var)
    ## search for library
    find_library (CASAPY_${_var}_LIBRARY ${_libboost}-mt-${CASAPY_BOOST_VERSION}
      PATHS ${CASAPY_BASEDIR} ${CASAPY_FRAMEWORKS}
      NO_DEFAULT_PATH
      )
    ## accumulate list of libraries
    if (CASAPY_${_var}_LIBRARY)
      list (APPEND CASAPY_BOOST_LIBRARIES ${CASAPY_${_var}_LIBRARY})
    endif (CASAPY_${_var}_LIBRARY)
  endforeach (_libboost)
  
  ##_____________________________________________________________________________
  ##                                                                      CFITSIO
    
  find_library (CASAPY_CFITSIO_LIBRARY cfitsio
    PATHS ${CASAPY_BASEDIR} ${CASAPY_FRAMEWORKS}
    NO_DEFAULT_PATH
    )

  ##_____________________________________________________________________________
  ##                                                                          PNG
  
  set (CASAPY_PNG_VERSION 3.25.0)
  
  find_library (CASAPY_PNG_LIBRARY png.${CASAPY_PNG_VERSION}
    PATHS ${CASAPY_BASEDIR} ${CASAPY_FRAMEWORKS}
    NO_DEFAULT_PATH
    )

  ##_____________________________________________________________________________
  ##                                                                       Python
  
  set (CASAPY_PYTHON_VERSION 2.5)
  
  find_path (CASAPY_PYTHON_INCLUDES Python.h
    PATHS ${CASAPY_BASEDIR} ${CASAPY_FRAMEWORKS}
    PATH_SUFFIXES Python.framework/Versions/${CASAPY_PYTHON_VERSION}/include/python${CASAPY_PYTHON_VERSION}
    NO_DEFAULT_PATH
    )
    
  find_file (CASAPY_PYTHON_LIBRARY Python
    PATHS ${CASAPY_BASEDIR} ${CASAPY_FRAMEWORKS}
    PATH_SUFFIXES
    Python.framework/Versions/${CASAPY_PYTHON_VERSION}
    NO_DEFAULT_PATH
    )

  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (CASAPY_INCLUDES casa.h
    PATHS ${include_locations}
    PATH_SUFFIXES casa
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (CASAPY_LIBRARIES libcasa
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (CASAPY_INCLUDES AND CASAPY_LIBRARIES)
    set (HAVE_CASAPY TRUE)
  else (CASAPY_INCLUDES AND CASAPY_LIBRARIES)
    set (HAVE_CASAPY FALSE)
    if (NOT CASAPY_FIND_QUIETLY)
      if (NOT CASAPY_INCLUDES)
	message (STATUS "Unable to find CASAPY header files!")
      endif (NOT CASAPY_INCLUDES)
      if (NOT CASAPY_LIBRARIES)
	message (STATUS "Unable to find CASAPY library files!")
      endif (NOT CASAPY_LIBRARIES)
    endif (NOT CASAPY_FIND_QUIETLY)
  endif (CASAPY_INCLUDES AND CASAPY_LIBRARIES)
  
  if (HAVE_CASAPY)
    if (NOT CASAPY_FIND_QUIETLY)
      message (STATUS "Found components for CASAPY")
      message (STATUS "CASAPY_BASEDIR         = ${CASAPY_BASEDIR}")
      message (STATUS "CASAPY_FRAMEWORKS      = ${CASAPY_FRAMEWORKS}")
      message (STATUS "CASAPY_INCLUDES        = ${CASAPY_INCLUDES}")
      message (STATUS "CASAPY_LIBRARIES       = ${CASAPY_LIBRARIES}")
      message (STATUS "CASAPY_BOOST_INCLUDES  = ${CASAPY_BOOST_INCLUDES}")
      message (STATUS "CASAPY_BOOST_LIBRARY   = ${CASAPY_BOOST_LIBRARY}")
      message (STATUS "CASAPY_PYTHON_INCLUDES = ${CASAPY_PYTHON_INCLUDES}")
      message (STATUS "CASAPY_BOOST_LIBRARIES = ${CASAPY_BOOST_LIBRARIES}")
      message (STATUS "CASAPY_PNG_LIBRARY     = ${CASAPY_PNG_LIBRARY}")
    endif (NOT CASAPY_FIND_QUIETLY)
  else (HAVE_CASAPY)
    if (CASAPY_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find CASAPY!")
    endif (CASAPY_FIND_REQUIRED)
  endif (HAVE_CASAPY)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    CASAPY_BASEDIR
    CASAPY_FRAMEWORKS
    CASAPY_INCLUDES
    CASAPY_LIBRARIES
    )
  
endif (NOT FIND_CASAPY_CMAKE)
