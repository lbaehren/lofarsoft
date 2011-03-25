
##_______________________________________________________________________________
## Additional settings to locate MPI installation

list (APPEND CMAKE_SYSTEM_PREFIX_PATH  /opt/openmpi/1.3.2)

##_______________________________________________________________________________
## Include standard CMake module

if (NOT MPI_FOUND)
  include (FindMPI)
endif (NOT MPI_FOUND)

##_______________________________________________________________________________
## Check if search was successful

## Search for MPI compiler

if (NOT MPI_COMPILER)
  message (STATUS "[PULSAR] Running extra check to locate MPI compiler...")
  find_program (MPI_COMPILER
    NAMES mpic++ mpicxx mpiCC mpicc
    HINTS ${_MPI_PREFIX_PATH}
    PATH_SUFFIXES bin
    )
endif (NOT MPI_COMPILER)

## Search for MPI header files

if (NOT MPI_INCLUDE_PATH)
  message (STATUS "[PULSAR] Running extra check to locate MPI include path...")
  find_path (MPI_INCLUDE_PATH mpi.h
    HINTS ${_MPI_PREFIX_PATH}
    PATH_SUFFIXES include
    )
endif (NOT MPI_INCLUDE_PATH)

## Search for MPI library

if (NOT MPI_LIBRARY)
  message (STATUS "[PULSAR] Running extra check to locate MPI library...")
  find_library (MPI_LIBRARY mpi
    HINTS ${_MPI_PREFIX_PATH}
    PATH_SUFFIXES lib
    )
endif (NOT MPI_LIBRARY)

##_______________________________________________________________________________
## Final check on variables expected to be set

set (MPI_FOUND TRUE)

if (NOT MPI_COMPILER)
  message (WARNING "[PULSAR] Missing MPI compiler!")
  set (MPI_FOUND FALSE)
endif (NOT MPI_COMPILER)

if (NOT MPI_LIBRARY)
  message (WARNING "[PULSAR] Missing MPI library!")
  set (MPI_FOUND FALSE)
endif (NOT MPI_LIBRARY)

if (NOT MPI_INCLUDE_PATH)
  message (WARNING "[PULSAR] Missing MPI include path!")
  set (MPI_FOUND FALSE)
endif (NOT MPI_INCLUDE_PATH)
