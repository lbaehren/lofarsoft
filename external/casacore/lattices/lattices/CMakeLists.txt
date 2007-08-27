
## -----------------------------------------------------------------------------
## liblattices

SET (lattices_modules
  LatticeMath
  Lattices
  )

## collect the source files in the modules

foreach (lattices_module ${lattices_modules})
  FILE (GLOB lattices_${lattices_module} ${lattices_module}/*.cc ${lattices_module}/*.tcc)
  LIST(APPEND lattices_sources ${lattices_${lattices_module}})
endforeach (lattices_module)

## build the library

add_library (casa_lattices
  ${lattices_sources}
)
target_link_libraries (casa_lattices casa_tables casa_scimath casa_scimath_f casa_casa
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  ## build up the list of directories ...
  foreach (lattices_module ${lattices_modules})
    list (APPEND tests "${lattices_module}/test")
  endforeach (lattices_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS casa_lattices
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
