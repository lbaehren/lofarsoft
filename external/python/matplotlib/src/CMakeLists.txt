
## ------------------------------------------------------------------------------
## Search for external components

## Numarray

find_path (arrayobject_h arrayobject.h
  PATHS ${include_locations}
  PATH_SUFFIXES
  python
  python/numarray
  )

if (arrayobject_h)
  string (REGEX REPLACE "include/python/numarray" "include/python" NUMARRAY_INCLUDES ${arrayobject_h})
  include_directories (NUMARRAY_INCLUDES)
endif (arrayobject_h)

## summary for external components

message (STATUS "NUMARRAY_INCLUDES = ${NUMARRAY_INCLUDES}")

## assemble the listof source files

file (GLOB matplotlib_sources *.c)

add_definitions (
  -DNUMARRAY=1
  -DNUMERIC=0
  -DPY_ARRAY_TYPES_PREFIX=0
)

add_library (matplotlib ${matplotlib_sources})

target_link_libraries (matplotlib
  libfreetype
  libgtk
  libpng
  libz
  )