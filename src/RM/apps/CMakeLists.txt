
add_executable (rmsynth main.cpp)

target_link_libraries (rmsynth
  rm
  ${rm_link_libraries}
  )

install (
  TARGETS rmsynth
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
