
## ------------------------------------------------------------------------------
## Search for external components

## assemble the listof source files

file (GLOB matplotlib_sources *.c)

add_definitions (
  -DNUMARRAY=1
  -DNUMERIC=0
  -DPY_ARRAY_TYPES_PREFIX=0
)

add_library (matplotlib ${matplotlib_sources})

target_link_libraries (matplotlib
  ${matplotlib_link_libraries}
  libfreetype
  libpng
  libz
  )