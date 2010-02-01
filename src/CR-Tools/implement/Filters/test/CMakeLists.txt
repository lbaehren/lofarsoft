
file (GLOB Filters_test t*.cc)

foreach (src ${Filters_test})

  ## strip file extension from name
  get_filename_component (src ${src} NAME_WE)

  ## compiler instructions
  add_executable (${src} ${src}.cc)

  ## linker instructions
  target_link_libraries (${src}
    cr
    ${cr_link_libraries}
    )

  ## installation instructions
  install (
    TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
  
endforeach (src)
  