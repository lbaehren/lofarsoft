## ------------------------------------------------------------------------------
## System header files

set (INCLUDE_DIRS
  /usr/include
  /usr/local/include
  )

find_path (HAVE_STDIO_H stdio.h ${INCLUDE_DIRS})
find_path (HAVE_STDLIB_H stdlib.h ${INCLUDE_DIRS})
find_path (HAVE_STRING_H string.h ${INCLUDE_DIRS})

message (STATUS "stdio.h  : ${HAVE_STDIO_H}")
message (STATUS "stdlib.h : ${HAVE_STDLIB_H}")
message (STATUS "string.h : ${HAVE_STRING_H}")

## ------------------------------------------------------------------------------
## Paths to header files

if (HAVE_STDIO_H)
  INCLUDE_DIRECTORIES (. .. ${HAVE_STDIO_H})
else (HAVE_STDIO_H)
  INCLUDE_DIRECTORIES (. ..)
endif (HAVE_STDIO_H)


## ------------------------------------------------------------------------------
## Create the library

file (GLOB libwcs_sources *.c)

add_library (wcs ${libwcs_sources})

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS wcs
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
