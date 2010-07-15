## =========================================================================
##
## The Transient Template Library
##
## =========================================================================

##__________________________________________________________
## Options

option (TTL_BUILD_TESTS "Build TTL tests" YES)
option (TTL_WITH_BINDINGS "Build TTL bindings" YES)

mark_as_advanced (TTL_BUILD_TESTS)
mark_as_advanced (TTL_WITH_BINDINGS)

##__________________________________________________________
## Lists of header files

file (GLOB ttl_headers *.h)

## ------------------------------------------------------------------------------
## Installation

install (FILES ${ttl_headers}
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

