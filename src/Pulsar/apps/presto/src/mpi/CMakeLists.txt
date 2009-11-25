
## ==============================================================================
##
##  Project settings
##
## ==============================================================================

if (MPI_COMPILER)
  set (ENV{CC} ${MPI_COMPILER})
endif (MPI_COMPILER)

SET (CMAKE_C_COMPILER_INIT mpicc)
#SET (CMAKE_CXX_COMPILER_INIT mpic++)

## Name of the project
project (presto_mpi C)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

## List of binaries to build

set (presto_mpi_binaries
  mpiprepsubband
)

## List of source components

set (mpiprepsubband_sources
  mpiprepsubband_cmd.c
  mpiprepsubband_utils.c
  mpiprepsubband.c
  )

foreach (src ${INSTRUMENTOBJS})
  list (APPEND mpiprepsubband_sources ${presto_mpi_SOURCE_DIR}/../${src})
endforeach (src)

add_executable (mpiprepsubband ${mpiprepsubband_sources})

## Build instructions for the binaries

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

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "+------------------------------------------------------------+")
message (STATUS "Project name       = ${PROJECT_NAME}")
message (STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message (STATUS "C compiler         = ${CMAKE_C_COMPILER}")
message (STATUS "+------------------------------------------------------------+")
