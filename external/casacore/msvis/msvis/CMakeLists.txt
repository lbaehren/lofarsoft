
## -----------------------------------------------------------------------------
## libmsvis

SET (msvis_modules
  MSVis
  )

## collect the source files in the modules

foreach (msvis_module ${msvis_modules})
  FILE (GLOB msvis_${msvis_module} ${msvis_module}/*.cc ${msvis_module}/*.tcc)
  LIST (APPEND msvis_sources ${msvis_${msvis_module}})
endforeach (msvis_module)

## -----------------------------------------------------------------------------
## build the library

add_library (casa_msvis
  ${msvis_sources}
)
target_link_libraries (casa_msvis casa_ms)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  ## build up the list of directories ...
  foreach (msvis_module ${msvis_modules})
    list (APPEND tests "${msvis_module}/test")
  endforeach (msvis_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS casa_msvis
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB msvis_headers *.h)
install (FILES ${msvis_headers}
  DESTINATION include/casacore/msvis
  )

foreach (msvis_module ${msvis_modules})
  ## assemble list of header files
  FILE (GLOB msvis_headers ${msvis_module}/*.h ${msvis_module}/*.tcc)
  ## set installation location
  install (FILES ${msvis_headers}
    DESTINATION include/casacore/msvis/${msvis_module}
    )
endforeach (msvis_module)
