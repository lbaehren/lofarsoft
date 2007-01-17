
PROJECT (CASA_measures_libmeasures)

## -----------------------------------------------------------------------------
## Library build

SET (measures_modules
  Measures
  TableMeasures
  )

## collect the source files in the modules

foreach (measures_module ${measures_modules})
  FILE (GLOB measures_${measures_module} ${measures_module}/*.cc)
endforeach (measures_module)

## collect the files responsible for template instantiation

FILE (GLOB measures_templates ../../templates/measures/*.cc)

## build the library

add_library (measures
  ${measures_Measures}
  ${measures_TableMeasures}
  ## -- Template instantiation
  ${measures_templates}
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (measures_module ${measures_modules})
  LIST (APPEND tests "${measures_module}/test")
endforeach (measures_module)

SUBDIRS (${tests})
