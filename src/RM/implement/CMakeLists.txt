
##__________________________________________________________
## Define the list of modules

set (rm_modules
  rmLib
  rmSim
  rmClean
  rmSynthesis
  )

if (RM_ENABLE_ITPP AND HAVE_ITPP)
  list (APPEND rm_modules rmWiener)
endif (RM_ENABLE_ITPP AND HAVE_ITPP)

##__________________________________________________________
## Process the contents of the modules directories

foreach (rm_module ${rm_modules})
  ## Add include paths
  include_directories (${RM_SOURCE_DIR}/implement/${rm_module})
  ## list of source files for this module
  FILE (GLOB rm_${rm_module}_sources ${rm_module}/*.cpp ${rm_module}/*.cc)
  ## list of header files for this module
  FILE (GLOB rm_${rm_module}_headers ${rm_module}/*.h)
  ## expand the list of source files
  list (APPEND rm_sources ${rm_${rm_module}_sources})
  list (APPEND rm_headers ${rm_${rm_module}_headers})
endforeach (rm_module)

##____________________________________________________________________
## Build the library

add_library (rm STATIC ${rm_sources})

target_link_libraries (rm ${rm_link_libraries})

##____________________________________________________________________
## Installation instructions

install (
  TARGETS rm
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

