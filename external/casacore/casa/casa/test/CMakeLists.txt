
set (casa_casa_test
  tTypes
  )

foreach (src ${casa_casa_test})
  ## compiler instructions 
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} casa)
endforeach (src)
