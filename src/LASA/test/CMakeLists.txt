
foreach (lasa_test 
    test_landau_distri
    test_root_file
    )
  
  ## compiler instructions
  add_executable (${lasa_test} ${lasa_test}.cxx)

  ## linker instructions
  target_link_libraries (${lasa_test}
    lasa
    ${lasa_link_libraries}
    )

  add_test (${lasa_test} ${lasa_test})
  
endforeach (lasa_test)
