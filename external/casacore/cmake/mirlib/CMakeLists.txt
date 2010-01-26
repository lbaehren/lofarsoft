## ------------------------------------------------------------------------------
## System header files

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

target_link_libraries (mir casa_casa)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS mir
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
