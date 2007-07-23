
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

add_library (measures
  ${measures_sources}
)
target_link_libraries (measures tables scimath)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (measures_module ${measures_modules})
    list (APPEND tests "${measures_module}/test")
  endforeach (measures_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS measures
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
