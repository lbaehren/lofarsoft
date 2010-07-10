
## -----------------------------------------------------------------------------
## Subdirectories

#add_subdirectory (lib)
add_subdirectory (src)
add_subdirectory (testing)

## Build "libcblas" first

FILE (GLOB cblas_sources src/*.f src/*.c)

add_library (cblas ${cblas_sources})

## -----------------------------------------------------------------------------
## Test programs

#add_executable (cblas_example1 cblas_example1.c)
#set_target_properties (cblas_example1
#  PROPERTIES
#  COMPILE_FLAGS -fPIC -O2 -fPIC
#  LINKER_LANGUAGE Fortran
#  )
#
#target_link_libraries (cblas_example1 cblas blas ${G2C_LIBRARIES})

#add_executable (cblas_example2 cblas_example2.c)
#set_target_properties (cblas_example2
#  PROPERTIES
#  COMPILE_FLAGS -fPIC -O2 -fPIC
#  LINKER_LANGUAGE Fortran
#  )
#target_link_libraries (cblas_example2 cblas blas ${G2C_LIBRARIES})
