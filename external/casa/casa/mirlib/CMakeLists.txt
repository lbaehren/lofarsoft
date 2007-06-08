PROJECT (libmir)

## -----------------------------------------------------------------------------
## Additional required external software


## -----------------------------------------------------------------------------
## libmir

## collect the source files

FILE (GLOB libmir_sources *.c)

## build the library

add_library (mir
  ${libmir_sources}
)
target_link_libraries (mir casa)
