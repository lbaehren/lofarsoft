
project (BLAS Fortran C)

include_directories (
  .
  ..
  cblas/src
  )

## -----------------------------------------------------------------------------
## Build "libblas" first

FILE (GLOB blas_sources *.f *.c)

add_library (blas ${blas_sources})

## -----------------------------------------------------------------------------
## Build "libcblas"

add_definitions (-O3 -DADD_)

FILE (GLOB cblas_sources cblas/src/*.f cblas/src/*.c)
add_library (cblas ${cblas_sources})
target_link_libraries (cblas blas)

## test programs

add_executable (cblas_example1 cblas/cblas_example1.c)
target_link_libraries (cblas_example1 blas cblas)

