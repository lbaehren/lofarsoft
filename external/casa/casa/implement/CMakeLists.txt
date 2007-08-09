PROJECT (CASA_casa_libcasa)

## -----------------------------------------------------------------------------
## Additional required external software

find_library (libm m
  PATHS
  /usr/lib
  /usr/local/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  /sw/lib
  )

## -----------------------------------------------------------------------------
## libcasa

SET (casa_modules
  Arrays
  BasicMath
  BasicSL
  Containers
  Exceptions
  IO
  Inputs
  Logging
  OS
  Quanta
  System
  Utilities
  )

## collect the source files in the modules

foreach (casa_module ${casa_modules})
  FILE (GLOB casa_${casa_module} ${casa_module}/*.cc)
  LIST(APPEND casa_sources ${casa_${casa_module}})
endforeach (casa_module)

## collect the files responsible for template instantiation

file (GLOB casa_templates ../../templates/casa/*.cc)

## build the library

add_library (casa
  aips.cc
  version2.cc
  ${casa_sources}
  ${casa_templates}
)
target_link_libraries (casa ${libm})

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASA_BUILD_TESTS)
  ## Assemble the list of directories ...
  foreach (casa_module ${casa_modules})
    LIST (APPEND tests "${casa_module}/test")
  endforeach (casa_module)
  ## ... and register it
  SUBDIRS (${tests})
endif (CASA_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation of libraries

install (TARGETS casa
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
