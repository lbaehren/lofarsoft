## ------------------------------------------------------------------------------
## System header files

set (INCLUDE_DIRS
  /usr/include
  /usr/local/include
  )

## ------------------------------------------------------------------------------
## Paths to header files

if (HAVE_STDIO_H)
  INCLUDE_DIRECTORIES (. .. ${HAVE_STDIO_H})
else (HAVE_STDIO_H)
  INCLUDE_DIRECTORIES (. ..)
endif (HAVE_STDIO_H)


## ------------------------------------------------------------------------------
## Create the library

add_library (mir
  bug.c
  dio.c
  headio.c
  hio.c
  key.c
  maskio.c
  pack.c
  scrio.c
  uvio.c
  xyio.c
  xyzio.c
  )

target_link_libraries (mir casa)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS mir
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
