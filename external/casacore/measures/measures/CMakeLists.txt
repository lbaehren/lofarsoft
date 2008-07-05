##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## libmeasures

SET (measures_modules
  Measures
  TableMeasures
  )

## collect the source files in the modules

foreach (measures_module ${measures_modules})
  FILE (GLOB measures_${measures_module} ${measures_module}/*.cc ${measures_module}/*.tcc)
  LIST(APPEND measures_sources ${measures_${measures_module}})
endforeach (measures_module)

## build the library

add_library (casa_measures
  ${measures_sources}
)
target_link_libraries (casa_measures casa_tables casa_scimath)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (measures_module ${measures_modules})
    add_subdirectory (${measures_module}/test)
  endforeach (measures_module)
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS casa_measures
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
