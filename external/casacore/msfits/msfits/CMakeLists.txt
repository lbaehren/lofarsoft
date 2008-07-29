
## -----------------------------------------------------------------------------
## libmsfits

SET (msfits_modules
  MSFits
  )

## collect the source files in the modules

foreach (msfits_module ${msfits_modules})
  FILE (GLOB msfits_${msfits_module} ${msfits_module}/*.cc ${msfits_module}/*.tcc)
  LIST (APPEND msfits_sources ${msfits_${msfits_module}})
endforeach (msfits_module)

## -----------------------------------------------------------------------------
## build the library

add_library (casa_msfits
  ${msfits_sources}
)
target_link_libraries (casa_msfits casa_ms casa_fits)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  ## build up the list of directories ...
  foreach (msfits_module ${msfits_modules})
    list (APPEND tests "${msfits_module}/test")
  endforeach (msfits_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation

## Library

install (TARGETS casa_msfits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB msfits_headers *.h)
install (FILES ${msfits_headers}
  DESTINATION include/casacore/msfits
  )

foreach (msfits_module ${msfits_modules})
  ## assemble list of header files
  FILE (GLOB msfits_headers ${msfits_module}/*.h ${msfits_module}/*.tcc)
  ## set installation location
  install (FILES ${msfits_headers}
    DESTINATION include/casacore/msfits/${msfits_module}
    )
endforeach (msfits_module)
