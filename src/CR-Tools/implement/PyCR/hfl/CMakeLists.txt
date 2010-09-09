## =========================================================================
##
## The Heino Falcke Library
##
## =========================================================================

##__________________________________________________________
## Options

option (HFL_BUILD_TESTS "Build HFL tests" YES)
option (HFL_WITH_BINDINGS "Build HFL bindings" YES)

mark_as_advanced (HFL_BUILD_TESTS)
mark_as_advanced (HFL_WITH_BINDINGS)

##__________________________________________________________
## Lists of header files

file (GLOB hfl_headers *.h)

## ------------------------------------------------------------------------------
## Installation

install (FILES ${hfl_headers}
  DESTINATION include/hfl
  )

## ------------------------------------------------------------------------------
## Tests

if (HFL_BUILD_TESTS)
  add_subdirectory (test)
endif (HFL_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Bindings

if (HFL_WITH_BINDINGS)
    add_subdirectory (bindings)
endif (HFL_WITH_BINDINGS)

