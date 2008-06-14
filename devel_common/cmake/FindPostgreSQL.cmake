# +-----------------------------------------------------------------------------+
# | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                               $ |
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

# - Check for the presence of PostgreSQL
#
# The following variables are set when PostgreSQL is found:
#  HAVE_PostgreSQL       = Set to true, if all components of PostgreSQL
#                          have been found.
#  POSTGRESQL_INCLUDES   = Include path for the header files of PostgreSQL
#  POSTGRESQL_LIBRARIES  = Link these to use PostgreSQL
#  PostgreSQL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for system header files included from within the library

find_path (stdio_h stdio.h ${include_locations})
find_path (stdlib_h stdlib.h ${include_locations})
find_path (string_h string.h ${include_locations})
find_path (stddef_h stddef.h ${include_locations})
find_path (stdarg_h stdarg.h ${include_locations})
find_path (strings_h strings.h ${include_locations})
find_path (SupportDefs_h SupportDefs.h ${include_locations})

if (strings_h)
  add_definitions (-DHAVE_STRINGS_H=1)
else (strings_h)
  add_definitions (-DHAVE_STRINGS_H=0)
endif (strings_h)

if (SupportDefs_h)
  add_definitions (-DHAVE_SUPPORTDEFS_H=1)
else (SupportDefs_h)
  add_definitions (-DHAVE_SUPPORTDEFS_H=0)
endif (SupportDefs_h)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (PostgreSQL_INCLUDES postgres.h postgres_ext.h pgtypes_numeric.h libpq-fe.h
  PATHS ${include_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library components

set (PostgreSQL_LIBRARIES "")

## [1] libpq

find_library (libpq pq
  PATHS ${lib_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

if (libpq)
  list (APPEND PostgreSQL_LIBRARIES ${libpq})
endif (libpq)

## [2] libpgtypes

find_library (libpgtypes pgtypes 
  PATHS ${lib_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

if (libpgtypes)
  list (APPEND PostgreSQL_LIBRARIES ${libpgtypes})
endif (libpgtypes)

## [3] libecpg

find_library (libecpg ecpg
  PATHS ${lib_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

if (libecpg)
  list (APPEND PostgreSQL_LIBRARIES ${libecpg})
endif (libecpg)

## [4] libpgport

find_library (libpgport pgport
  PATHS ${lib_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

if (libpgport)
  list (APPEND PostgreSQL_LIBRARIES ${libpgport})
endif (libpgport)

## adjust the ordering of the libraries during linking

if (PostgreSQL_LIBRARIES)
  list (REVERSE PostgreSQL_LIBRARIES)
endif (PostgreSQL_LIBRARIES)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (PostgreSQL_INCLUDES AND PostgreSQL_LIBRARIES)
  set (HAVE_PostgreSQL TRUE)
else (PostgreSQL_INCLUDES AND PostgreSQL_LIBRARIES)
  set (HAVE_PostgreSQL FALSE)
  if (NOT PostgreSQL_FIND_QUIETLY)
    if (NOT PostgreSQL_INCLUDES)
      message (STATUS "Unable to find PostgreSQL header files!")
    endif (NOT PostgreSQL_INCLUDES)
    if (NOT PostgreSQL_LIBRARIES)
      message (STATUS "Unable to find PostgreSQL library files!")
    endif (NOT PostgreSQL_LIBRARIES)
  endif (NOT PostgreSQL_FIND_QUIETLY)
endif (PostgreSQL_INCLUDES AND PostgreSQL_LIBRARIES)

if (HAVE_PostgreSQL)
  if (NOT PostgreSQL_FIND_QUIETLY)
    message (STATUS "Found components for PostgreSQL")
    message (STATUS "PostgreSQL_INCLUDES  = ${PostgreSQL_INCLUDES}")
    message (STATUS "PostgreSQL_LIBRARIES = ${PostgreSQL_LIBRARIES}")
  endif (NOT PostgreSQL_FIND_QUIETLY)
else (HAVE_PostgreSQL)
  if (PostgreSQL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PostgreSQL!")
  endif (PostgreSQL_FIND_REQUIRED)
endif (HAVE_PostgreSQL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  PostgreSQL_INCLUDES
  PostgreSQL_LIBRARIES
  )
