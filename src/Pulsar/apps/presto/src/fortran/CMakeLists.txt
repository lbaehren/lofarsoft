
enable_language(Fortran)

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/src/fortran)

file (GLOB fort_sources least_squares.f)

add_library (fort SHARED ${fort_sources})

if (UNIX)
 if (APPLE)
   set_target_properties (fort
     PROPERTIES
     SUFFIX .dylib
     COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
     LINK_FLAGS "-dynamiclib -fno-second-underscore"
     )
 else (APPLE)
   set_target_properties (fort
     PROPERTIES
     SUFFIX .so
     COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
     LINK_FLAGS "-shared -fno-second-underscore"
     )
 endif (APPLE)
endif (UNIX)

##set_target_properties (fort
##  PROPERTIES
##  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
##  LINK_FLAGS "-fno-second-underscore"
##  )

install (
  TARGETS fort
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
