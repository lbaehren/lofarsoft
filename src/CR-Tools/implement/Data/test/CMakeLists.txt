##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Tests without extra requirements
##
## ==============================================================================

##____________________________________________________________________
## Set up the list with test program sources

file (GLOB tests *.cc *.cpp)

##____________________________________________________________________
## Build the test programs

foreach (src ${tests})
  
  ## extract the components of the filename
  get_filename_component (_name ${src} NAME_WE)
  get_filename_component (_ext ${src} EXT)
  ## compiler instructions
  add_executable(${_name} ${_name}${_ext})
  ## linker instructions
  target_link_libraries (${_name} cr ${cr_link_libraries})
  ## testing
  add_test (${_name} ${_name})
  ## installation instructions
  install (
    TARGETS ${_name}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
  
endforeach (src)
