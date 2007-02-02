
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

add_library(math ${srcs})
target_link_libraries (math ${BLITZ_LIBRARIES})
