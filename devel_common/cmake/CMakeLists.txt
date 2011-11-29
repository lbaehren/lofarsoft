
##
## Installation o the CMake modules for common usage
##

file (GLOB _cmakeFiles *.cmake)

## Installation instructions

install (
  FILES ${_cmakeFiles}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules
  )
