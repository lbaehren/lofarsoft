##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

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

add_library (casa_coordinates
  ${coordinates_sources}
)
target_link_libraries (casa_coordinates casa_fits ${LAPACK_LIBRARIES})

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (coordinates_module ${coordinates_modules})
    add_subdirectory (${coordinates_module}/test)
  endforeach (coordinates_module)
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation

## Library

install (TARGETS casa_coordinates
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB coordinates_headers *.h)
install (FILES ${coordinates_headers}
  DESTINATION include/casacore/coordinates
  )

foreach (coordinates_module ${coordinates_modules})
  ## assemble list of header files
  FILE (GLOB coordinates_headers ${coordinates_module}/*.h ${coordinates_module}/*.tcc)
  ## set installation location
  install (FILES ${coordinates_headers}
    DESTINATION include/casacore/coordinates/${coordinates_module}
    )
endforeach (coordinates_module)
