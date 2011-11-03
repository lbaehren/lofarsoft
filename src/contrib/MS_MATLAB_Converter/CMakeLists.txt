
add_executable (ms2matlab ms2matlab.cpp)
target_link_libraries (ms2matlab
  dsp
  ${CASA_LIBRARIES}
  )
