
PROJECT (CASA_coordinates_libcoordinates)

## -----------------------------------------------------------------------------
## libcoordinates

SET (coordinates_modules
  Coordinates
  )

## collect the source files in the modules

foreach (coordinates_module ${coordinates_modules})
  FILE (GLOB coordinates_${coordinates_module} ${coordinates_module}/*.cc)
  LIST(APPEND coordinates_sources ${coordinates_${coordinates_module}})
endforeach (coordinates_module)

## collect the files responsible for template instantiation

FILE (GLOB coordinates_templates ../../templates/coordinates/*.cc)

## build the library

ADD_LIBRARY (coordinates
  ${coordinates_sources}
  ${coordinates_templates}
)
target_link_libraries (coordinates tables scimath casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASA_BUILD_TESTS)
  ## Assemble the list of directories ...
  foreach (coordinates_module ${coordinates_modules})
    LIST (APPEND tests "${coordinates_module}/test")
  endforeach (coordinates_module)
  ## ... and register it
  SUBDIRS (${tests})
endif (CASA_BUILD_TESTS)
