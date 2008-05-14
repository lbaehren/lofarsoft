##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 654 2007-08-29 15:57:17Z baehren          $
##------------------------------------------------------------------------

# - Check for the presence of MySQL
#
# The following variables are set when MySQL is found:
#  HAVE_MYSQL       = Set to true, if all components of MySQL
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

find_path (MYSQL_INCLUDES mysql/mysql.h mysql/mysql_version.h
  PATHS ${include_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library components

set (MYSQL_LIBRARIES "")

## [1] libmysqlclient

find_library (libmysqlclient mysqlclient
  PATHS ${lib_locations}
  PATH_SUFFIXES mysql
  NO_DEFAULT_PATH
  )

if (libmysqlclient)
  list (APPEND MYSQL_LIBRARIES ${libmysqlclient})
endif (libmysqlclient)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MYSQL_INCLUDES AND MYSQL_LIBRARIES)
  set (HAVE_MYSQL TRUE)
else (MYSQL_INCLUDES AND MYSQL_LIBRARIES)
  set (HAVE_MYSQL FALSE)
  if (NOT MYSQL_FIND_QUIETLY)
    if (NOT MYSQL_INCLUDES)
      message (STATUS "Unable to find MySQL header files!")
    endif (NOT MYSQL_INCLUDES)
    if (NOT MYSQL_LIBRARIES)
      message (STATUS "Unable to find MySQL library files!")
    endif (NOT MYSQL_LIBRARIES)
  endif (NOT MYSQL_FIND_QUIETLY)
endif (MYSQL_INCLUDES AND MYSQL_LIBRARIES)

if (HAVE_MYSQL)
  if (NOT MYSQL_FIND_QUIETLY)
    message (STATUS "Found components for MySQL")
    message (STATUS "MYSQL_INCLUDES  = ${MYSQL_INCLUDES}")
    message (STATUS "MYSQL_LIBRARIES = ${MYSQL_LIBRARIES}")
  endif (NOT MYSQL_FIND_QUIETLY)
else (HAVE_MYSQL)
  if (MySQL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find MySQL!")
  endif (MySQL_FIND_REQUIRED)
endif (HAVE_MYSQL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MYSQL_INCLUDES
  MYSQL_LIBRARIES
  )
