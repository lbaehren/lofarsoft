
## -----------------------------------------------------------------------------
## libimages

SET (images_modules
  Images
  Wnbt
  )

## collect the source files in the modules

foreach (images_module ${images_modules})
  FILE (GLOB images_${images_module} ${images_module}/*.cc ${images_module}/*.tcc)
  LIST(APPEND images_sources ${images_${images_module}})
endforeach (images_module)

## build the library

add_library (images
  ${images_sources}
)
target_link_libraries (images components coordinates fits mir)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (images_module ${images_modules})
    LIST (APPEND tests "${images_module}/test")
  endforeach (images_module)
  ## ... and register them
  SUBDIRS (${tests})
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS images
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
