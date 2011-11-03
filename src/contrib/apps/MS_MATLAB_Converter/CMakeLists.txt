
##_______________________________________________________________________________
## System instruction

if (NOT CASACORE_FOUND)
  find_package (CASACORE)
endif (NOT CASACORE_FOUND)

if (NOT MATLAB_FOUND)
  find_package (Matlab)
endif (NOT MATLAB_FOUND)

##_______________________________________________________________________________
## Build instructions

if (CASACORE_FOUND AND MATLAB_FOUND)

  include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CASACORE_INCLUDES}
    ${MATLAB_INCLUDES}
    )
  
  ## Compiler instructions
  add_executable (ms2matlab
    Antenna.cpp
    MSInfo.cpp
    MSReader.cpp
    MatWriter.cpp
    ms2matlab.cpp
    )
  ## Linker instructions
  target_link_libraries (ms2matlab ${CASA_LIBRARIES})
  
else (CASACORE_FOUND AND MATLAB_FOUND)
  
  message (STATUS "[ms2matlab] Unable to build application - unresolved dependencies!")
  message (STATUS " .. Have casacore = ${CASACORE_FOUND}")
  message (STATUS " .. Have Matlab   = ${MATLAB_FOUND}")
  
endif (CASACORE_FOUND AND MATLAB_FOUND)
