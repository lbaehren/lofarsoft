
## -----------------------------------------------------------------------------
## Compiler settings

include_directories (
  .
  ..
  )

## -----------------------------------------------------------------------------
## Directory containing the test programs

subdirs (test)

## -----------------------------------------------------------------------------
## Library build

FILE (GLOB srcs *.cc)

add_library(imaging ${srcs})
target_link_libraries (imaging ${BLITZ_LIBRARY})
