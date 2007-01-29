## CASA/implement/lattices

PROJECT (CASA_lattices_liblattices)

## -----------------------------------------------------------------------------
## liblattices

SET (lattices_modules
  LatticeMath
  Lattices
  )

## collect the source files in the modules

foreach (lattices_module ${lattices_modules})
  FILE (GLOB lattices_${lattices_module} ${lattices_module}/*.cc)
  LIST(APPEND lattices_sources ${lattices_${lattices_module}})
endforeach (lattices_module)

## collect the files responsible for template instantiation

FILE (GLOB lattices_templates ../../templates/lattices/*.cc)

## build the library

ADD_LIBRARY (lattices
  ${lattices_sources}
  ${lattices_templates}
)
target_link_libraries (lattices casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (lattices_module ${lattices_modules})
  LIST (APPEND tests "${lattices_module}/test")
endforeach (lattices_module)

SUBDIRS (${tests})
