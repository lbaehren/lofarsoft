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

# - Check for the presence of DAL version 1
#
# The following variables are set when DAL1 is found:
#
#   DAL1_FOUND      = Set to true, if all components of DAL1 have been found.
#   DAL1_INCLUDES   = Include path for the header files of DAL1
#   DAL1_LIBRARIES  = Link these to use DAL1
#   DAL1_LFLAGS     = Linker flags (optional)
#   DAL1_VERSION_MAJOR = Major version number
#   DAL1_VERSION_MINOR = Minor version number
#   DAL1_VERSION_PATCH = Patch version number
#   DAL1_VERSION       = DAL1 version string
#
# In order to maintain compatibility with the Filesystem Hierarchy Standard (FHS)
# the following default installation layout has been defined:
#
# /opt
# └── dal                          DAL1_INSTALL_PREFIX
#     ├── bin                      DAL1_INSTALL_BINDIR
#     ├── include                  DAL1_INSTALL_INCLUDEDIR
#     │   ├── coordinates
#     │   ├── core
#     │   ├── data_common
#     │   └── data_hl
#     ├── lib                      DAL1_INSTALL_INCLUDEDIR
#     └── share                    DAL1_INSTALL_DATAROOTDIR
#         └── doc                  DAL1_INSTALL_DOCDIR
#             └── html
#

if (NOT DAL1_FOUND)

  if (NOT DAL1_ROOT_DIR)
    set (DAL1_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT DAL1_ROOT_DIR)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (DAL1_INCLUDES dal_config.h
    HINTS ${DAL1_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${include_locations}
    PATH_SUFFIXES include include/dal
    )

  ## core/dalDataset.h

  find_path (DAL1_DALDATASET_H core/dalDataset.h
    HINTS ${DAL1_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${include_locations}
    PATH_SUFFIXES include include/dal
    )
  if (DAL1_DALDATASET_H)
    list (APPEND DAL1_INCLUDES ${DAL1_DALDATASET_H})
  endif (DAL1_DALDATASET_H)
  
  ## core/dalDataset.h

  find_path (DAL1_COORDINATE_H coordinates/Coordinate.h
    HINTS ${DAL1_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${include_locations}
    PATH_SUFFIXES include include/dal
    )
  if (DAL1_COORDINATE_H)
    list (APPEND DAL1_INCLUDES ${DAL1_COORDINATE_H})
  endif (DAL1_COORDINATE_H)

  ## Remove duplicate entries

  list (REMOVE_DUPLICATES DAL1_INCLUDES)
  
  ##_____________________________________________________________________________
  ## Check for the library

  set (DAL1_LIBRARIES "")
  
  find_library (DAL1_DAL_LIBRARY dal
    HINTS ${DAL1_ROOT_DIR}
    PATHS /sw /usr /usr/local /opt/dal ${lib_locations}
    PATH_SUFFIXES lib lib/dal
    )
  if (DAL1_DAL_LIBRARY)
    list (APPEND DAL1_LIBRARIES ${DAL1_DAL_LIBRARY})
  endif (DAL1_DAL_LIBRARY)
  
  ##_____________________________________________________________________________
  ## Check for the executable(s)
  
  foreach (_dal_executable
      lopes2h5
      msread
      ms2h5
      tbb2h5
      TBBraw2h5
      )

    ## try to locate the executable
    find_program (DAL1_${_dal_executable}_EXECUTABLE ${_dal_executable}
      HINTS ${DAL1_ROOT_DIR}
      PATH_SUFFIXES bin bin/dal
      )
    
  endforeach (_dal_executable)
  
  ##_____________________________________________________________________________
  ## Test DAL1 library for:
  ##  - library version <major.minor.release>
  ##  - registered external packages (e.g. casacore, HDF5, etc.)
  
  if (DAL1_INCLUDES AND DAL1_LIBRARIES)
    ## Locate test program
    find_file (HAVE_TestDAL1Library TestDAL1Library.cc
      PATHS ${PROJECT_SOURCE_DIR}
      PATH_SUFFIXES cmake devel_common/cmake Modules
      )
    ## Build and run test program
    if (HAVE_TestDAL1Library)
      try_run(DAL1_VERSION_RUN_RESULT DAL1_VERSION_COMPILE_RESULT
	${PROJECT_BINARY_DIR}
	${HAVE_TestDAL1Library}
#	CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${DAL1_LIBRARIES}
	COMPILE_DEFINITIONS -I${DAL1_INCLUDES}
	RUN_OUTPUT_VARIABLE DAL1_VERSION_OUTPUT
	)
    endif (HAVE_TestDAL1Library)
  endif (DAL1_INCLUDES AND DAL1_LIBRARIES)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found

  if (DAL1_INCLUDES AND DAL1_LIBRARIES)
    set (DAL1_FOUND TRUE)

    ## __________________________________________________________________________
    ## Find library version
    file (STRINGS "${DAL1_INCLUDES}/dal_config.h" DAL1_H REGEX "^#define DAL1_VERSION \"[^\"]*\"$")
    string (REGEX REPLACE "^.*DAL1_VERSION \"([0-9]+).*$" "\\1" DAL1_VERSION_MAJOR "${DAL1_H}")
    string (REGEX REPLACE "^.*DAL1_VERSION \"[0-9]+\\.([0-9]+).*$" "\\1" DAL1_VERSION_MINOR  "${DAL1_H}")
    string (REGEX REPLACE "^.*DAL1_VERSION \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" DAL1_VERSION_PATCH "${DAL1_H}")
    set (DAL1_VERSION "${DAL1_VERSION_MAJOR}.${DAL1_VERSION_MINOR}.${DAL1_VERSION_PATCH}")

  else (DAL1_INCLUDES AND DAL1_LIBRARIES)
    set (DAL1_FOUND FALSE)
    if (NOT DAL1_FIND_QUIETLY)
      if (NOT DAL1_INCLUDES)
	message (STATUS "Unable to find DAL1 header files!")
      endif (NOT DAL1_INCLUDES)
      if (NOT DAL1_LIBRARIES)
	message (STATUS "Unable to find DAL1 library files!")
      endif (NOT DAL1_LIBRARIES)
    endif (NOT DAL1_FIND_QUIETLY)
  endif (DAL1_INCLUDES AND DAL1_LIBRARIES)
  
  if (DAL1_FOUND)
    if (NOT DAL1_FIND_QUIETLY)
      message (STATUS "Found components for DAL1")
      message (STATUS "DAL1_ROOT_DIR  = ${DAL1_ROOT_DIR}")
      message (STATUS "DAL1_INCLUDES  = ${DAL1_INCLUDES}")
      message (STATUS "DAL1_LIBRARIES = ${DAL1_LIBRARIES}")
      message (STATUS "DAL1_VERSION   = ${DAL1_VERSION}")
    endif (NOT DAL1_FIND_QUIETLY)
  else (DAL1_FOUND)
    if (DAL1_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find DAL1!")
    endif (DAL1_FIND_REQUIRED)
  endif (DAL1_FOUND)

  ## Compatibility settings
  
  if (DAL1_FOUND)
    set (HAVE_DAL1 TRUE)
  else (DAL1_FOUND)
    set (HAVE_DAL1 FALSE)
  endif (DAL1_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    DAL1_INCLUDES
    DAL1_LIBRARIES
    )
  
endif (NOT DAL1_FOUND)
