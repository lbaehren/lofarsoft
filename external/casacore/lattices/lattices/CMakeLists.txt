
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

add_library (lattices
  ${lattices_sources}
)
target_link_libraries (lattices tables scimath)
add_dependencies (lattices tables scimath)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (lattices_module ${lattices_modules})
    list (APPEND tests "${lattices_module}/test")
  endforeach (lattices_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_tests)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS lattices
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
