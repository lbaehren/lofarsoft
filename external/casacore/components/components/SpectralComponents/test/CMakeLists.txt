
## ------------------------------------------------------------------------------
## Test programs for components/ComponentModels

SET (tests
  tProfileFit1D
  tSpectralFit
)

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} components coordinates fits measures tables scimath scimath_f casa ${G2C_LIBRARIES})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
