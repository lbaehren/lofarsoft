
file (GLOB fbdsm_includes *.inc)

foreach (_inc ${fbdsm_includes})
  ## Store the name of the file
  get_filename_component(_filename ${_inc} NAME)
  ## read in file contents ...
  file (READ ${_inc} _code)
  ## ... and write file in new location
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/${_filename} ${_code})
endforeach (_inc)