
add_executable (lasa_analysis main_analysis.cxx)

target_link_libraries (lasa_analysis
  lasa
  ${lasa_link_libraries}
  )