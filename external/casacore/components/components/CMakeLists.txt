##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## libcomponents

SET (components_modules
  ComponentModels
  SpectralComponents
  )

## collect the source files in the modules

foreach (components_module ${components_modules})
  FILE (GLOB components_${components_module} ${components_module}/*.cc ${components_module}/*.tcc)
  LIST(APPEND components_sources ${components_${components_module}})
endforeach (components_module)

## build the library

add_library (casa_components
  ${components_sources}
)
target_link_libraries (casa_components casa_coordinates ${LAPACK_LIBRARIES})

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (components_module ${components_modules})
    add_subdirectory (${components_module}/test)
  endforeach (components_module)
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation

## Library

install (TARGETS casa_components
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB components_headers *.h)
install (FILES ${components_headers}
  DESTINATION include/casacore/components
  )

foreach (components_module ${components_modules})
  ## assemble list of header files
  FILE (GLOB components_headers ${components_module}/*.h)
  ## set installation location
  install (FILES ${components_headers}
    DESTINATION include/casacore/components/${components_module}
    )
endforeach (components_module)
