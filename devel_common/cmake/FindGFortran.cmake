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

# - Check for the presence of GFORTRAN, G2C and F2C. The search for the libraries
#   is combined into a single CMake search script, since this will allow us to
#   also do some cross-checking if required.
#
# The following variables are set when GFORTRAN is found:
#
#  HAVE_GFORTRAN        = Set to true, if all components of GFORTRAN have been
#                         found.
#  GFORTRAN_EXECUTABLE  = Excutable of the GFortran compiler
#  GFORTRAN_INCLUDES    = Include path for the header files of GFORTRAN
#  GFORTRAN_LIBRARY     = Path to libgfortran
#  GFORTRAN_LIBRARY_DIR = Directory within which libgfortran is located
#  GFORTRAN_LFLAGS      = Linker flags (optional)
#
# The following variables are set when G2C is found:
#
#  HAVE_G2C       = Set to true, if all components of G2C have been found.
#  G2C_INCLUDES   = Include path for the header files of G2C
#  G2C_LIBRARIES  = Link these to use G2C
#  G2C_LFLAGS     = Linker flags (optional)
#
# The following variables are set when F2C is found:
#
#  HAVE_F2C       = Set to true, if all components of F2C have been found.
#  F2C_INCLUDES   = Include path for the header files of F2C
#  F2C_LIBRARIES  = Link these to use F2C
#  F2C_LFLAGS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

if (UNIX)
  if (APPLE)
    ## architecture-dependent locations
    IF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      list (APPEND lib_locations
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.0
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.1
	)
    ELSE (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      list (APPEND lib_locations
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin8/4.0.0
	/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin8/4.0.1
	)
    ENDIF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
    ## generic locations (Fink installation)
    list (APPEND lib_locations
      /sw/lib/gcc4.4/lib
      /sw/lib/gcc4.3/lib
      /sw/lib/gcc4.2/lib
      /sw/lib/gcc4.1/lib
      )
  else (APPLE)
    list (APPEND lib_locations
      /usr/lib/gcc/i486-linux-gnu/4.4
      /usr/lib/gcc/i486-linux-gnu/4.3
      /usr/lib/gcc/i486-linux-gnu/4.2
      /usr/lib/gcc/i486-linux-gnu/4.1
      /usr/lib/gcc/i386-redhat-linux/3.4.6
      /usr/lib64/gcc/x86_64-suse-linux/4.3
      /usr/lib/gcc/x86_64-redhat-linux/3.4.3/32
      /usr/lib/gcc/x86_64-redhat-linux/3.4.3
      /usr/lib/gcc/x86_64-redhat-linux/4.1.1
      /usr/lib/gcc/x86_64-redhat-linux/4.1.2
      /usr/lib/gcc/x86_64-linux-gnu/4.2
      )
  endif (APPLE)
endif (UNIX)

## -----------------------------------------------------------------------------
## Check for GFortran

## Executable

find_program (GFORTRAN_EXECUTABLE gfortran gfortran-4.3 gfortran-4.2  gfortran-4.1
  PATHS ${bin_locations}
  )

## Header files

find_path (GFORTRAN_INCLUDES libgfortran.h
  PATHS
  ${include_locations}
  /usr/lib/gcc/x86_64-redhat-linux
  PATH_SUFFIXES
  libgfortran
  4.1.2/finclude
  NO_DEFAULT_PATH
  )

## Library

find_library (GFORTRAN_LIBRARY gfortran
  PATHS ${lib_locations}
  /usr/local/gfortran/lib
  /opt/local/lib/gcc43
  /usr/local/gfortran/lib/x86_64
  NO_DEFAULT_PATH
  )

if (GFORTRAN_LIBRARY)
  get_filename_component(GFORTRAN_LIBRARY_DIR ${GFORTRAN_LIBRARY} PATH)
else (GFORTRAN_LIBRARY)
  message (STATUS "[FindGFortran] Unable to locate libgfortran!")
endif (GFORTRAN_LIBRARY)

## -----------------------------------------------------------------------------
## Check for G2C

## Header files

find_path (G2C_INCLUDES g2c.h
  PATHS
  ${include_locations}
  /usr/lib/gcc
  /usr/lib/gcc-lib
  PATH_SUFFIXES
  i486-linux/3.3.5/include
  i586-suse-linux/3.3.3/include
  i586-suse-linux/3.3.4/include
  i586-suse-linux/3.3.5/include
  x86_64-redhat-linux/3.4.3/include
  i386-redhat-linux/3.4.6/include
  )

## Library

find_library (G2C_LIBRARY g2c
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

if (G2C_LIBRARY)
  get_filename_component(G2C_LIBRARY_DIR ${G2C_LIBRARY} PATH)
else (G2C_LIBRARY)
  message (STATUS "[FindGFortran] Unable to locate libg2c!")
endif (G2C_LIBRARY)

## -----------------------------------------------------------------------------
## Check for F2C

## Header files

find_path (F2C_INCLUDES f2c.h
  PATHS ${include_locations} /usr/lib/gcc /usr/lib/gcc-lib
  PATH_SUFFIXES
  i486-linux/3.3.5/include
  i586-suse-linux/3.3.3/include
  i586-suse-linux/3.3.4/include
  i586-suse-linux/3.3.5/include
  x86_64-redhat-linux/3.4.3/include
  i386-redhat-linux/3.4.6/include
  )

## Library 

find_library (F2C_LIBRARY f2c
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (GFORTRAN_LIBRARY)
  set (GFORTRAN_LIBRARIES ${GFORTRAN_LIBRARY})
else (GFORTRAN_LIBRARY)
  if (G2C_LIBRARY)
    set (GFORTRAN_LIBRARIES ${G2C_LIBRARY})
  else (G2C_LIBRARY)
    if (F2C_LIBRARY)
      set (GFORTRAN_LIBRARIES ${F2C_LIBRARY})
    endif (F2C_LIBRARY)
  endif (G2C_LIBRARY)
endif (GFORTRAN_LIBRARY)

## GFortran

if (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARY)
  set (HAVE_GFORTRAN TRUE)
else (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARY)
  set (HAVE_GFORTRAN FALSE)
  if (NOT GFORTRAN_FIND_QUIETLY)
    if (NOT GFORTRAN_INCLUDES)
      message (STATUS "Unable to find GFortran header files!")
    endif (NOT GFORTRAN_INCLUDES)
    if (NOT GFORTRAN_LIBRARY)
      message (STATUS "Unable to find GFortran library files!")
    endif (NOT GFORTRAN_LIBRARY)
  endif (NOT GFORTRAN_FIND_QUIETLY)
endif (GFORTRAN_INCLUDES AND GFORTRAN_LIBRARY)

if (HAVE_GFORTRAN)
  if (NOT GFORTRAN_FIND_QUIETLY)
    message (STATUS "Found components for GFortran")
    message (STATUS "GFORTRAN_EXECUTABLE = ${GFORTRAN_EXECUTABLE}")
    message (STATUS "GFORTRAN_INCLUDES   = ${GFORTRAN_INCLUDES}")
    message (STATUS "GFORTRAN_LIBRARY    = ${GFORTRAN_LIBRARY}")
  endif (NOT GFORTRAN_FIND_QUIETLY)
else (HAVE_GFORTRAN)
  if (GFORTRAN_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find GFortran!")
  endif (GFORTRAN_FIND_REQUIRED)
endif (HAVE_GFORTRAN)

## g2c

if (G2C_INCLUDES AND G2C_LIBRARY)
  set (HAVE_G2C TRUE)
else (G2C_INCLUDES AND G2C_LIBRARY)
  set (HAVE_G2C FALSE)
  if (NOT GFORTRAN_FIND_QUIETLY)
    if (NOT G2C_INCLUDES)
      message (STATUS "Unable to find G2C header files!")
    endif (NOT G2C_INCLUDES)
    if (NOT G2C_LIBRARY)
      message (STATUS "Unable to find G2C library files!")
    endif (NOT G2C_LIBRARY)
  endif (NOT GFORTRAN_FIND_QUIETLY)
endif (G2C_INCLUDES AND G2C_LIBRARY)

if (HAVE_G2C)
  if (NOT GFORTRAN_FIND_QUIETLY)
    message (STATUS "Found components for G2C")
    message (STATUS "G2C_INCLUDES  = ${G2C_INCLUDES}")
    message (STATUS "G2C_LIBRARY   = ${G2C_LIBRARY}")
  endif (NOT GFORTRAN_FIND_QUIETLY)
else (HAVE_G2C)
  if (G2C_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find G2C!")
  endif (G2C_FIND_REQUIRED)
endif (HAVE_G2C)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  GFORTRAN_INCLUDES
  GFORTRAN_LIBRARY
  GFORTRAN_LIBRARY_DIR
  G2C_INCLUDES
  G2C_LIBRARY
  G2C_LIBRARY_DIR
  F2C_INCLUDES
  F2C_LIBRARY
  )
