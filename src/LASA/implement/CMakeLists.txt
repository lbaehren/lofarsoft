
## Build up the list of source files

file (GLOB liblasa_sources *.cxx)

if (liblasa_sources)
  ## Compiler instructions
  add_library (lasa ${liblasa_sources})
  ## Linker instructions
  target_link_libraries (lasa ${lasa_link_libraries})
  ## Installation instructions
  install (
    TARGETS lasa
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )  
else (liblasa_sources)
  message (WARNING "[LASA] Unable to build library - missing sources!")
endif (liblasa_sources)


