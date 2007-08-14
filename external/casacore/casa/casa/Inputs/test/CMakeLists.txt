
## ------------------------------------------------------------------------------
## Test programs for casa/Inputs

SET (tests
tInput
tParam
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
