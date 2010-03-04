
##__________________________________________________________
## Assemble list of source files

if (HAVE_ITPP)
  list (APPEND rm_tests trmnoise.cpp)
endif (HAVE_ITPP)

foreach (src ${rm_tests})
  ## get the name of the file without the extension
  get_filename_component (src ${src} NAME_WE)
  get_filename_component (extension ${src} EXT)
  ## compiler instructions
  add_executable(${src} ${src}${extension})
  ## linker instructions
  target_link_libraries(${src} rm ${rm_link_libraries})
  ## installation instructions
  install (TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
endforeach (src)
