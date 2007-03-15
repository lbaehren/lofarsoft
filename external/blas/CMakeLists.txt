
## -----------------------------------------------------------------------------
##
##  blas
##  `-- cblas
##      |-- lib
##      |-- src
##      `-- test
##
## -----------------------------------------------------------------------------

project (BLAS Fortran C)

subdirs (
  cblas
  )

include_directories (
  .
  ..
  cblas
  cblas/src
  cblas/testing
  )

## -----------------------------------------------------------------------------
## Required external packages

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
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
target_link_libraries (cblas ${libg2c} blas)

## cblas/*.c

add_executable (cblas_example1 cblas/cblas_example1.c)
target_link_libraries (cblas_example1 blas cblas)

#add_executable (cblas_example2 cblas/cblas_example2.c)
#target_link_libraries (cblas_example1 blas cblas)
