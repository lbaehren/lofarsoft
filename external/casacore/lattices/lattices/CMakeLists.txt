
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
  ${casacore_BINARY_DIR}/casacore.h
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

## -----------------------------------------------------------------------------
## Installation

## Library

install (TARGETS casa_lattices
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB lattices_headers *.h)
install (FILES ${lattices_headers}
  DESTINATION include/casacore/lattices
  )

foreach (lattices_module ${lattices_modules})
  ## assemble list of header files
  FILE (GLOB lattices_headers ${lattices_module}/*.h ${lattices_module}/*.tcc)
  ## set installation location
  install (FILES ${lattices_headers}
    DESTINATION include/casacore/lattices/${lattices_module}
    )
endforeach (lattices_module)
