PROJECT (CMAKE_TEST)

## -----------------------------------------------------------------------------
## Test file for the various CMake find scripts.

MESSAGE (STATUS "Testing FindBlitz ++++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindBlitz.cmake)

MESSAGE (STATUS "Testing FindBoost ++++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindBoost.cmake)

MESSAGE (STATUS "Testing FindCASA +++++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindCASA.cmake)

MESSAGE (STATUS "Testing FindCFITSIO ++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindCFITSIO.cmake)

MESSAGE (STATUS "Testing FindCPGPLOT ++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindCPGPLOT.cmake)

MESSAGE (STATUS "Testing FindHDF5 +++++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindHDF5.cmake)

MESSAGE (STATUS "Testing FindROOT +++++++++++++++++++++++++++++++++++++++++++++")

INCLUDE (./FindROOT.cmake)
