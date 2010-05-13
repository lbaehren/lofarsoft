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

# - Check for the presence of SSL
#
# The following variables are set when SSL is found:
#  HAVE_SSL       = Set to true, if all components of SSL have been found.
#  SSL_INCLUDES   = Include path for the header files of SSL
#  SSL_LIBRARIES  = Link these to use SSL
#  SSL_LFLAGS     = Linker flags (optional)

if (NOT FIND_SSL_CMAKE)
  
  set (FIND_SSL_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (SSL_INCLUDES crypto.h ssl.h ssl2.h
    PATHS ${include_locations}
    PATH_SUFFIXES ssl openssl
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (SSL_LIBRARIES ssl
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (SSL_INCLUDES AND SSL_LIBRARIES)
    set (HAVE_SSL TRUE)
  else (SSL_INCLUDES AND SSL_LIBRARIES)
    set (HAVE_SSL FALSE)
    if (NOT SSL_FIND_QUIETLY)
      if (NOT SSL_INCLUDES)
	message (STATUS "Unable to find SSL header files!")
      endif (NOT SSL_INCLUDES)
      if (NOT SSL_LIBRARIES)
	message (STATUS "Unable to find SSL library files!")
      endif (NOT SSL_LIBRARIES)
    endif (NOT SSL_FIND_QUIETLY)
  endif (SSL_INCLUDES AND SSL_LIBRARIES)
  
  if (HAVE_SSL)
    if (NOT SSL_FIND_QUIETLY)
      message (STATUS "Found components for SSL")
      message (STATUS "SSL_INCLUDES  = ${SSL_INCLUDES}")
      message (STATUS "SSL_LIBRARIES = ${SSL_LIBRARIES}")
    endif (NOT SSL_FIND_QUIETLY)
  else (HAVE_SSL)
    if (SSL_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find SSL!")
    endif (SSL_FIND_REQUIRED)
  endif (HAVE_SSL)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    SSL_INCLUDES
    SSL_LIBRARIES
    )
  
endif (NOT FIND_SSL_CMAKE)
