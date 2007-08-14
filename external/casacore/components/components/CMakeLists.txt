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

add_library (components
  ${components_sources}
)
target_link_libraries (components coordinates ${LAPACK_LIBRARIES})

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  foreach (components_module ${components_modules})
    add_subdirectory (${components_module}/test)
  endforeach (components_module)
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS components
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
