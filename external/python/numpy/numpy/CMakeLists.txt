
##_______________________________________________________________________________
## Project subdirectories

foreach (numpy_module core lib linalg)
  ## add subdirectory to the list of project directories
  add_subdirectory (${numpy_module})
  ## add location of header files for module
  include_directories (${numpy_module}/include)
  include_directories (${numpy_module}/src)
endforeach (numpy_module)

