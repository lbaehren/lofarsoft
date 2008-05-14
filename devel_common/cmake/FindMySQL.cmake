##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 654 2007-08-29 15:57:17Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of MySQL
#
# The following variables are set when MySQL is found:
#  HAVE_MySQL       = Set to true, if all components of MySQL
#                          have been found.
#  MYSQL_INCLUDES   = Include path for the header files of MySQL
#  MYSQL_LIBRARIES  = Link these to use MySQL
#  MySQL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

include (CMakeSettings)

## -----------------------------------------------------------------------------
## Check for system header files included from within the library


## -----------------------------------------------------------------------------
## Check for the header files

find_path (MySQL_INCLUDES mysql.h mysql_com.h mysql_version.h
  PATHS ${include_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library components

set (MySQL_LIBRARIES "")

## [1] libmysys

find_library (libmysys mysys
  PATHS ${lib_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

if (libmysys)
  list (APPEND MySQL_LIBRARIES ${libmysys})
endif (libmysys)

## [2] libmystrings

find_library (libmystrings mystrings 
  PATHS ${lib_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

if (libmystrings)
  list (APPEND MySQL_LIBRARIES ${libmystrings})
endif (libmystrings)

## [3] libmysqlclient

find_library (libmysqlclient mysqlclient
  PATHS ${lib_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

if (libmysqlclient)
  list (APPEND MySQL_LIBRARIES ${libmysqlclient})
endif (libmysqlclient)

## [4] libvio

find_library (libvio vio
  PATHS ${lib_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

if (libvio)
  list (APPEND MySQL_LIBRARIES ${libvio})
endif (libvio)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MySQL_INCLUDES AND MySQL_LIBRARIES)
  set (HAVE_MySQL TRUE)
else (MySQL_INCLUDES AND MySQL_LIBRARIES)
  set (HAVE_MySQL FALSE)
  if (NOT MySQL_FIND_QUIETLY)
    if (NOT MySQL_INCLUDES)
      message (STATUS "Unable to find MySQL header files!")
    endif (NOT MySQL_INCLUDES)
    if (NOT MySQL_LIBRARIES)
      message (STATUS "Unable to find MySQL library files!")
    endif (NOT MySQL_LIBRARIES)
  endif (NOT MySQL_FIND_QUIETLY)
endif (MySQL_INCLUDES AND MySQL_LIBRARIES)

if (HAVE_MySQL)
  if (NOT MySQL_FIND_QUIETLY)
    message (STATUS "Found components for MySQL")
    message (STATUS "MySQL_INCLUDES  = ${MySQL_INCLUDES}")
    message (STATUS "MySQL_LIBRARIES = ${MySQL_LIBRARIES}")
  endif (NOT MySQL_FIND_QUIETLY)
else (HAVE_MySQL)
  if (MySQL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find MySQL!")
  endif (MySQL_FIND_REQUIRED)
endif (HAVE_MySQL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MySQL_INCLUDES
  MySQL_LIBRARIES
  )
