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

# - Check for the presence of PostgreSQL
#
# The following variables are set when PostgreSQL is found:
#  HAVE_POSTGRESQL       = Set to true, if all components of PostgreSQL
#                          have been found.
#  POSTGRESQL_INCLUDES   = Include path for the header files of PostgreSQL
#  POSTGRESQL_LIBRARIES  = Link these to use PostgreSQL
#  POSTGRESQL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for system header files included from within the library

find_path (HAVE_STDIO_H       stdio.h        PATHS ${include_locations})
find_path (HAVE_STDLIB_H      stdlib.h       PATHS ${include_locations})
find_path (HAVE_STRING_H      string.h       PATHS ${include_locations})
find_path (HAVE_STDDEF_H      stddef.h       PATHS ${include_locations})
find_path (HAVE_STDARG_H      stdarg.h       PATHS ${include_locations})
find_path (HAVE_STRINGS_H     strings.h      PATHS ${include_locations})
find_path (HAVE_SUPPORTDEFS_H SupportDefs.h  PATHS ${include_locations})

## -----------------------------------------------------------------------------
## Check for the header files

find_path (POSTGRESQL_INCLUDES postgres.h postgres_ext.h pgtypes_numeric.h libpq-fe.h
  PATHS ${include_locations}
  PATH_SUFFIXES postgresql
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library components

set (POSTGRESQL_LIBRARIES "")

## [1] libpq

find_library (HAVE_LIBPQ pq
  PATHS ${lib_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  NO_DEFAULT_PATH
  )

if (HAVE_LIBPQ)
  list (APPEND POSTGRESQL_LIBRARIES ${HAVE_LIBPQ})
endif (HAVE_LIBPQ)

## [2] libpgtypes

find_library (HAVE_LIBPGTYPES pgtypes 
  PATHS ${lib_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  NO_DEFAULT_PATH
  )

if (HAVE_LIBPGTYPES)
  list (APPEND POSTGRESQL_LIBRARIES ${HAVE_LIBPGTYPES})
endif (HAVE_LIBPGTYPES)

## [3] libecpg

find_library (libecpg ecpg
  PATHS ${lib_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  NO_DEFAULT_PATH
  )

if (libecpg)
  list (APPEND POSTGRESQL_LIBRARIES ${libecpg})
endif (libecpg)

## [4] libpgport

find_library (libpgport pgport
  PATHS ${lib_locations}
  PATH_SUFFIXES
  postgresql
  pgsql
  NO_DEFAULT_PATH
  )

if (libpgport)
  list (APPEND POSTGRESQL_LIBRARIES ${libpgport})
endif (libpgport)

## adjust the ordering of the libraries during linking

if (POSTGRESQL_LIBRARIES)
  list (REVERSE POSTGRESQL_LIBRARIES)
endif (POSTGRESQL_LIBRARIES)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)
  set (HAVE_POSTGRESQL TRUE)
else (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)
  set (HAVE_POSTGRESQL FALSE)
  if (NOT POSTGRESQL_FIND_QUIETLY)
    if (NOT POSTGRESQL_INCLUDES)
      message (STATUS "Unable to find PostgreSQL header files!")
    endif (NOT POSTGRESQL_INCLUDES)
    if (NOT POSTGRESQL_LIBRARIES)
      message (STATUS "Unable to find PostgreSQL library files!")
    endif (NOT POSTGRESQL_LIBRARIES)
  endif (NOT POSTGRESQL_FIND_QUIETLY)
endif (POSTGRESQL_INCLUDES AND POSTGRESQL_LIBRARIES)

if (HAVE_POSTGRESQL)
  if (NOT POSTGRESQL_FIND_QUIETLY)
    message (STATUS "Found components for PostgreSQL")
    message (STATUS "PostgreSQL_INCLUDES  = ${POSTGRESQL_INCLUDES}")
    message (STATUS "PostgreSQL_LIBRARIES = ${POSTGRESQL_LIBRARIES}")
  endif (NOT POSTGRESQL_FIND_QUIETLY)
else (HAVE_POSTGRESQL)
  if (PostgreSQL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find PostgreSQL!")
  endif (PostgreSQL_FIND_REQUIRED)
endif (HAVE_POSTGRESQL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  POSTGRESQL_INCLUDES
  POSTGRESQL_LIBRARIES
  HAVE_LIBPQ
  HAVE_LIBPGTYPES
  )
