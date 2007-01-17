
PROJECT (CASA_ms_libms)

## -----------------------------------------------------------------------------
## Library build

SET (ms_modules
  MeasurementSets
  MSPlot
  )

## collect the source files in the modules

foreach (ms_module ${ms_modules})
  FILE (GLOB ms_${ms_module} ${ms_module}/*.cc)
endforeach (ms_module)

## collect the files responsible for template instantiation

FILE (GLOB ms_templates ../../templates/ms/*.cc)

## build the library

add_library (ms
  ${ms_MeasurementSets}
  ${ms_MSPlot}
  ## -- Template instantiation
  ${ms_templates}
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

SUBDIRS (MeasurementSets)
