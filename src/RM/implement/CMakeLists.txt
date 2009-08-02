
set (rm_sources
  rm.cpp
  rmCube.cpp
  rmio.cpp
  )

add_library (rm  ${rm_sources})

install (
  TARGETS rm
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

