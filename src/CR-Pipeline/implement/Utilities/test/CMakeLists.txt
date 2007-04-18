
include_directories (. .. ../..)

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tMasking
  tParameterFromRecord
  tProgressBar
  tStringTools
  tTimestamp
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr)
endforeach (src)

## ------------------------------------------------------------------------------
## Additional test programs which require extra care

#add_executable (table2root table2root.cc)
#target_link_libraries (table2root ${ROOT_LIBRARIES})

