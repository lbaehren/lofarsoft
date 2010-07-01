##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

##__________________________________________________________
## Options

option (TTL_BUILD_TESTS "Build TTL tests" NO)
option (TTL_WITH_BINDINGS "Build TTL bindings" NO)

##__________________________________________________________
## Lists of header files

file (GLOB ttl_headers *.h)

## ------------------------------------------------------------------------------
## Installation

install (FILES ${dal_headers}
  DESTINATION include/ttl
  )

## ------------------------------------------------------------------------------
## Tests

if (TTL_BUILD_TESTS)
  add_subdirectory (test)
endif (TTL_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Bindings

if (TTL_WITH_BINDINGS)
    add_subdirectory (bindings)
endif (TTL_WITH_BINDINGS)

