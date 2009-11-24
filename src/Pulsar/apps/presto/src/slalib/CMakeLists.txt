
enable_language(Fortran)

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/src/slalib)

file (GLOB sla_sources *.f)
list (APPEND sla_sources gresid.F)
list (APPEND sla_sources random.F)

add_library (sla SHARED ${sla_sources})

if (UNIX)
 if (APPLE)
   set_target_properties (sla
     PROPERTIES
     SUFFIX .dylib
     COMPILE_FLAGS "-fPIC -g -O -fno-second-underscore"
     LINK_FLAGS "-dynamiclib -fno-second-underscore"
     )
 else (APPLE)
   set_target_properties (sla
     PROPERTIES
     SUFFIX .so
     COMPILE_FLAGS "-fPIC -g -O -fno-second-underscore"
     LINK_FLAGS "-shared -fno-second-underscore"
     )
 endif (APPLE)
endif (UNIX)

##set_target_properties (sla
##  PROPERTIES
##  COMPILE_FLAGS "-fPIC -g -O -fno-second-underscore"
##  LINK_FLAGS "-fno-second-underscore"
##  )

install (
  TARGETS sla
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
