
## List of test scripts

file (GLOB pycr_tests *.py)

## 

foreach (_pycr_test ${pycr_tests})
  ## get filename components
  get_filename_component (_test_name      ${_pycr_test} NAME_WE)
  get_filename_component (_test_extension ${_pycr_test} EXT)
  ## Add test
  add_test (pycr_${_test_name} ${PYTHON_EXECUTABLE} ${_pycr_test})
endforeach (_pycr_test)

## Installation

install (FILES ${pycr_tests}
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)
