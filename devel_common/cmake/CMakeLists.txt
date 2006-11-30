PROJECT (CMAKE_TEST)

## -----------------------------------------------------------------------------
## Test file for the various CMake find scripts.

message (STATUS "Testing FindBlitz ++++++++++++++++++++++++++++++++++++++++++++")

include (./FindBlitz.cmake)

message (STATUS "Testing FindBoost ++++++++++++++++++++++++++++++++++++++++++++")

include (./FindBoost.cmake)

message (STATUS "Testing FindCASA +++++++++++++++++++++++++++++++++++++++++++++")

include (./FindCASA.cmake)

message (STATUS "Testing FindCFITSIO ++++++++++++++++++++++++++++++++++++++++++")

include (./FindCFITSIO.cmake)

message (STATUS "Testing FindCPGPLOT ++++++++++++++++++++++++++++++++++++++++++")

include (./FindCPGPLOT.cmake)

message (STATUS "Testing FindHDF5 +++++++++++++++++++++++++++++++++++++++++++++")

include (./FindHDF5.cmake)

message (STATUS "Testing FindROOT +++++++++++++++++++++++++++++++++++++++++++++")

include (./FindROOT.cmake)

message (STATUS "Testing FindWCS ++++++++++++++++++++++++++++++++++++++++++++++")

include (./FindWCS.cmake)

message (STATUS "Testing completed ++++++++++++++++++++++++++++++++++++++++++++")
