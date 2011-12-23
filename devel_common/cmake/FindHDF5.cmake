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

# - Check for the presence of HDF5
#
# The following variables are set when HDF5 is found:
#  HAVE_HDF5             = Set to true, if all components of HDF5 have been found.
#  HDF5_INCLUDES         = Include path for the header files of HDF5
#  HDF5_HDF5_LIBRARY     = Path to libhdf5
#  HDF5_HDF5_HL_LIBRARY  = Path to libhdf5_hl, the high-level interface
#  HDF5_HDF5_CPP_LIBRARY = Path to libhdf5_cpp
#  HDF5_LIBRARIES        = Link these to use HDF5
#  HDF5_VERSION_MAJOR    = Major version of the HDF5 library
#  HDF5_VERSION_MINOR    = Minor version of the HDF5 library
#  HDF5_VERSION_RELEASE  = Release version of the HDF5 library

if (NOT FIND_HDF5_CMAKE)

  if (NOT HDF5_ROOT_DIR)
    set (HDF5_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT HDF5_ROOT_DIR)

  set (FIND_HDF5_CMAKE     TRUE )
  set (HDF5_VERSION_STRING 0    )

  ##_____________________________________________________________________________
  ## Check for the header files
  
  ## Include directory

  find_path (HDF5_INCLUDES hdf5.h hdf5_hl.h
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES include hdf5 include/hdf5
    NO_DEFAULT_PATH
    )

  ## Individual header files
  
  find_path (HAVE_HDF5_HDF5_H hdf5.h
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES include hdf5 include/hdf5
    NO_DEFAULT_PATH
    )
  
  find_path (HAVE_HDF5_H5LT_H H5LT.h
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES include hdf5 include/hdf5
    NO_DEFAULT_PATH
    )
  
  find_path (HAVE_HDF5_HDF5_HL_H hdf5_hl.h
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES include hdf5 include/hdf5
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library components
  
  set (HDF5_LIBRARIES "")

  ## Core library (libhdf5)
  
  find_library (HDF5_HDF5_LIBRARY hdf5
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES lib hdf5/lib
    NO_DEFAULT_PATH
    )
  
  if (HDF5_HDF5_LIBRARY)
    set (HDF5_LIBRARIES ${HDF5_HDF5_LIBRARY})
  endif (HDF5_HDF5_LIBRARY)
  
  ## High level interface (libhdf5_hl)
  
  FIND_LIBRARY (HDF5_HDF5_HL_LIBRARY hdf5_hl
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES lib hdf5/lib
    NO_DEFAULT_PATH
    )
  
  if (HDF5_HDF5_HL_LIBRARY)
    list (APPEND HDF5_LIBRARIES ${HDF5_HDF5_HL_LIBRARY})
  endif (HDF5_HDF5_HL_LIBRARY)
  
  ## C++ interface (libhdf5_cpp)
  
  FIND_LIBRARY (HDF5_HDF5_CPP_LIBRARY hdf5_cpp
    HINTS ${HDF5_ROOT_DIR}
    PATH_SUFFIXES lib hdf5/lib
    NO_DEFAULT_PATH
    )
  
  if (HDF5_HDF5_CPP_LIBRARY)
    list (APPEND HDF5_LIBRARIES ${HDF5_HDF5_CPP_LIBRARY})
  endif (HDF5_HDF5_CPP_LIBRARY)
  
  
  ##_____________________________________________________________________________
  ## Check for the executables
  
  find_program (H5CHECK_EXECUTABLE h5check
    HINTS ${HDF5_ROOT_DIR}
    NO_DEFAULT_PATH
    )
  
  find_program (H5DUMP_EXECUTABLE h5dump 
    HINTS ${HDF5_ROOT_DIR}
    NO_DEFAULT_PATH
    )
  
  find_program (H5LS_EXECUTABLE h5ls 
    HINTS ${HDF5_ROOT_DIR}
    NO_DEFAULT_PATH
    )
  
  find_program (H5STAT_EXECUTABLE h5stat 
    HINTS ${HDF5_ROOT_DIR}
    NO_DEFAULT_PATH
    )

  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (HDF5_INCLUDES AND HDF5_LIBRARIES)
    set (HAVE_HDF5  TRUE)
    set (HDF5_FOUND TRUE)
  else (HDF5_INCLUDES AND HDF5_LIBRARIES)
    set (HAVE_HDF5  FALSE)
    set (HDF5_FOUND FALSE)
    if (NOT HDF5_FIND_QUIETLY)
      if (NOT HDF5_INCLUDES)
	message (STATUS "Unable to find HDF5 header files!")
      endif (NOT HDF5_INCLUDES)
      if (NOT HDF5_LIBRARIES)
	message (STATUS "Unable to find HDF5 library files!")
      endif (NOT HDF5_LIBRARIES)
    endif (NOT HDF5_FIND_QUIETLY)
  endif (HDF5_INCLUDES AND HDF5_LIBRARIES)
  
  ##_____________________________________________________________________________
  ## Test HDF5 library for:
  ##  - library version <major.minor.release>
  ##  - parallel I/O support
  ##  - default API version
  
  if (HDF5_INCLUDES AND HDF5_LIBRARIES)
    ## Locate test program
    find_file (HAVE_TestHDF5Library TestHDF5Library.cc
      PATHS ${PROJECT_SOURCE_DIR} ${LUS_SOURCE_DIR}
      PATH_SUFFIXES cmake devel_common/cmake Modules
      )
    ## Build and run test program
    if (HAVE_TestHDF5Library)
      try_run(HDF5_VERSION_RUN_RESULT HDF5_VERSION_COMPILE_RESULT
	${PROJECT_BINARY_DIR}
	${HAVE_TestHDF5Library}
	CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${HDF5_LIBRARIES}
	COMPILE_DEFINITIONS -I${HDF5_INCLUDES}
	RUN_OUTPUT_VARIABLE HDF5_VERSION_OUTPUT
	)
    endif (HAVE_TestHDF5Library)
  endif (HDF5_INCLUDES AND HDF5_LIBRARIES)
  
  ## Comile of test program successful?
  if (HDF5_VERSION_COMPILE_RESULT)
    ## Run of test program successful?
    if (HDF5_VERSION_RUN_RESULT)

      ## Library version _________________________

      string(REGEX REPLACE "H5_VERS_MAJOR ([0-9]+).*" "\\1" HDF5_VERSION_MAJOR ${HDF5_VERSION_OUTPUT})
      string(REGEX REPLACE ".*H5_VERS_MINOR ([0-9]+).*" "\\1" HDF5_VERSION_MINOR ${HDF5_VERSION_OUTPUT})
      string(REGEX REPLACE ".*H5_VERS_RELEASE ([0-9]+).*" "\\1" HDF5_VERSION_RELEASE ${HDF5_VERSION_OUTPUT})

      ## Support for parallel I/O? _______________

      string(REGEX REPLACE ".*H5_HAVE_PARALLEL ([0-9]+).*" "\\1" HDF5_HAVE_PARALLEL_IO ${HDF5_VERSION_OUTPUT})

      ## Default API version _____________________

      string(REGEX REPLACE ".*H5_USE_16_API_DEFAULT ([0-9]+).*" "\\1" HDF5_USE_16_API_DEFAULT ${HDF5_VERSION_OUTPUT})
      
      ## Library version number string ___________

      if (HDF5_VERSION_MAJOR AND HDF5_VERSION_MINOR AND HDF5_VERSION_RELEASE)
	math (EXPR HDF5_VERSION_STRING '10000*${HDF5_VERSION_MAJOR}+100*${HDF5_VERSION_MINOR}+${HDF5_VERSION_RELEASE}')
      endif (HDF5_VERSION_MAJOR AND HDF5_VERSION_MINOR AND HDF5_VERSION_RELEASE)
  
    else (HDF5_VERSION_RUN_RESULT)
      message (STATUS "[HDF5] Failed to run TestHDF5Library!")
    endif (HDF5_VERSION_RUN_RESULT)
  else (HDF5_VERSION_COMPILE_RESULT)
    message (STATUS "[HDF5] Failed to compile TestHDF5Library!")
    set (HDF5_VERSION_MAJOR   "-1" )
    set (HDF5_VERSION_MINOR   "-1" )
    set (HDF5_VERSION_RELEASE "-1" )
    set (HDF5_HAVE_PARALLEL_IO FALSE)
  endif (HDF5_VERSION_COMPILE_RESULT)
  
  set (HDF5_VERSION "${HDF5_VERSION_MAJOR}.${HDF5_VERSION_MINOR}.${HDF5_VERSION_RELEASE}")
  
  ##_____________________________________________________________________________
  ## Feedback
  
  if (HAVE_HDF5)
    if (NOT HDF5_FIND_QUIETLY)
      message (STATUS "Found components for HDF5")
      message (STATUS "HDF5_INCLUDES        = ${HDF5_INCLUDES}")
      message (STATUS "HDF5_LIBRARIES       = ${HDF5_LIBRARIES}")
      message (STATUS "HDF5_VERSION_MAJOR   = ${HDF5_VERSION_MAJOR}")
      message (STATUS "HDF5_VERSION_MINOR   = ${HDF5_VERSION_MINOR}")
      message (STATUS "HDF5_VERSION_RELEASE = ${HDF5_VERSION_RELEASE}")
    endif (NOT HDF5_FIND_QUIETLY)
  else (HAVE_HDF5)
    if (HDF5_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find HDF5!")
    endif (HDF5_FIND_REQUIRED)
  endif (HAVE_HDF5)

  ## Set standard variable
  set (HDF5_FOUND ${HAVE_HDF5})
  
  ##_____________________________________________________________________________
  ## Mark as advanced ...
  
  mark_as_advanced (
    HDF5_INCLUDES
    HDF5_LIBRARIES
    HAVE_H5PUBLIC_H
    HDF5_VERSION
    HDF5_VERSION_MAJOR
    HDF5_VERSION_MINOR
    HDF5_VERSION_RELEASE
    HAVE_TESTHDF5VERSION
    HDF5_HDF5_LIBRARY
    HDF5_HDF5_HL_LIBRARY
    HDF5_HDF5_CPP_LIBRARY
    HDF5_IS_PARALLEL
    )
  
endif (NOT FIND_HDF5_CMAKE)
