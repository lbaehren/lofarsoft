
##_______________________________________________________________________________
##                                                           List of source files

file (GLOB tests_rm *.cpp)

##_______________________________________________________________________________
##                                                     Build/Install instructions

foreach (src ${tests_rm})

  ## Get the name of the file without the extension
  get_filename_component (src ${src} NAME_WE)
  ## Compiler instructions
  add_executable(${src} ${src}.cpp)  
  ## Linker instructions
  target_link_libraries (${src}
    rm
    ${rm_link_libraries}
    )
  ## Installation instructions
  install (
    TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endforeach (src)

