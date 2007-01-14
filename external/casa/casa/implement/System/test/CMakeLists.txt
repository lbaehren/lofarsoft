
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tAipsrc
  tAipsrcValue
  tAppInfo
  tChoice
  tObjectID
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

