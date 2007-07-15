
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
target_link_libraries (components coordinates)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (components_module ${components_modules})
  LIST (APPEND tests "${components_module}/test")
endforeach (components_module)

SUBDIRS (${tests})

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS components
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
