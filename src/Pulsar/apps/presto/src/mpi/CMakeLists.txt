
SET(CMAKE_C_COMPILER_INIT mpic++)
SET(CMAKE_CXX_COMPILER_INIT mpic++)

## Name of the project
project (presto_mpi)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## List of binaries to build

set (presto_mpi_binaries
  mpiprepsubband
)


## Build instructions for the binaries

add_executable (mpiprepsubband
  mpiprepsubband_cmd.c
  mpiprepsubband_utils.c
  mpiprepsubband.c
  ${INSTRUMENTOBJS}
  )

foreach (src ${presto_mpi_binaries})
  ## Target properties
  set_target_properties (${src}
    PROPERTIES
###    CMAKE_COMMAND ${MPI_COMPILER}
###    COMPILER_LANGUAGE ${MPI_COMPILER}
###    LINKER_LANGUAGE ${MPI_COMPILER}
    COMPILE_FLAGS "-g -O3 -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
  )
## Linker instructions
  target_link_libraries (${src} presto ${MPI_LIBRARIES} ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)
