
## Set up the list of source files
file (GLOB lasa_apps *.cxx)

foreach (src ${lasa_apps})

  ## extract the components of the filename
  get_filename_component (_name ${src} NAME_WE)
  get_filename_component (_ext ${src} EXT)
  ## compiler instructions
  add_executable(${_name} ${_name}${_ext})
  ## linker instructions
  target_link_libraries (${_name} lasa ${lasa_link_libraries})
  ## installation instructions
  install (
    TARGETS ${_name}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )  

endforeach (src)