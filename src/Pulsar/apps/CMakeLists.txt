## ==============================================================================
##
##  Top level Pulsar apps CMakeLists.txt file
##
## ==============================================================================

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

add_subdirectory (presto)

add_subdirectory (bf2format)

if (FFTW3_FFTW3F_LIBRARY)
  add_subdirectory (sigproc-4.3)
else (FFTW3_FFTW3F_LIBRARY)
  message (STATUS "[sigproc-4.3] Missing fftw3f library - skipping package!")
endif (FFTW3_FFTW3F_LIBRARY)

add_subdirectory (see)

add_subdirectory (fluxmaster)

add_subdirectory (lpps_search)