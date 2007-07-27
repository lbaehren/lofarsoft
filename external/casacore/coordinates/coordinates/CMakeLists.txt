
## -----------------------------------------------------------------------------
## libcoordinates

SET (coordinates_modules
  Coordinates
  )

## collect the source files in the modules

foreach (coordinates_module ${coordinates_modules})
  FILE (GLOB coordinates_${coordinates_module} ${coordinates_module}/*.cc ${coordinates_module}/*.tcc)
  LIST(APPEND coordinates_sources ${coordinates_${coordinates_module}})
endforeach (coordinates_module)

## build the library

add_library (coordinates
  ${coordinates_sources}
)
target_link_libraries (coordinates fits)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (coordinates_module ${coordinates_modules})
  LIST (APPEND tests "${coordinates_module}/test")
endforeach (coordinates_module)

SUBDIRS (${tests})

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS coordinates
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
