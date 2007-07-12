
message (STATUS "[CMake package configuration] casacore")

project (casacore)

subdirs (
  casa
  components
  coordinates
  fits
  images
  install
  lattices
  measures
  mirlib
  ms
  msfits
  msvis
  scimath
  tables
  )

## ------------------------------------------------------------------------------
## Dependency of the libraries

# casa        : --
# tables      : casa
# mirlib      : casa
# scimath     : casa
# measures    : tables scimath
# fits        : measures
# coordinates : fits
# components  : coordinates
# lattices    : tables scimath
# ms          : measures
# images      : components mirlib
# msfits      : ms fits
# msvis       : ms
