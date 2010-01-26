##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_IMAGES
  Images
  Regions
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_IMAGES ${CASACORE_MODULES_IMAGES})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_IMAGES} ${MODULE_IMAGES}/*.yy)
  list (APPEND MODULE_IMAGES_BISON ${bison_${MODULE_IMAGES}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_IMAGES} ${MODULE_IMAGES}/*.ll)
  list (APPEND MODULE_IMAGES_FLEX ${flex_${MODULE_IMAGES}})
  ## Source files
  file (GLOB images_${MODULE_IMAGES} ${MODULE_IMAGES}/*.cc ${MODULE_IMAGES}/*.tcc)
  list (APPEND MODULE_IMAGES_SOURCES ${images_${MODULE_IMAGES}})
endforeach (MODULE_IMAGES)

## Processing of Bison input files

foreach (bison_input ${MODULE_IMAGES_BISON})
  casacore_process_bison (${bison_input} ${MODULE_IMAGES_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_IMAGES_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_IMAGES_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_images SHARED ${MODULE_IMAGES_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_images STATIC ${MODULE_IMAGES_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions
target_link_libraries (casa_images
  mirlib
  casa_components
  casa_coordinates
  casa_fits
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  ${WCSLIB_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_images
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_IMAGES_HEADERS *.h)
install (FILES ${MODULES_IMAGES_HEADERS}
  DESTINATION include/casacore/images
  )

foreach (MODULE_IMAGES ${CASACORE_MODULES_IMAGES})
  ## assemble list of header files
  FILE (GLOB MODULES_IMAGES_HEADERS ${MODULE_IMAGES}/*.h ${MODULE_IMAGES}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_IMAGES_HEADERS}
    DESTINATION include/casacore/images/${MODULE_IMAGES}
    )
endforeach (MODULE_IMAGES)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_IMAGES ${CASACORE_MODULES_IMAGES})
    add_subdirectory (${MODULE_IMAGES}/test)
  endforeach (MODULE_IMAGES)
endif (CASACORE_ENABLE_TESTING)
