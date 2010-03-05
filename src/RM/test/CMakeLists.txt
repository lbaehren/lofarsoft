
##__________________________________________________________
## Assemble list of source files

file (GLOB rm_tests *.cc *.cpp)

if (NOT HAVE_ITPP)
  list (REMOVE_ITEM rm_tests trmnoise.cpp)
endif (NOT HAVE_ITPP)

if (NOT HAVE_FFTW3)
  list (REMOVE_ITEM rm_tests tPreshift.cpp)
endif (NOT HAVE_FFTW3)

##__________________________________________________________
## Build the test programs

foreach (src ${rm_tests})
  ## get the name of the file without the extension
  get_filename_component (src ${src} NAME_WE)
  get_filename_component (extension ${src} EXT)
  ## compiler instructions
  add_executable(${src} ${src}${extension})
  ## linker instructions
  target_link_libraries(${src} rm ${rm_link_libraries})
  ## Testing
  add_test (${src} ${src})
  ## installation instructions
  install (TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
endforeach (src)
