
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

add_library (msfits
  ${msfits_sources}
)
target_link_libraries (msfits ms fits)

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

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS msfits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
