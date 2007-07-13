PROJECT (VLT)

INCLUDE_DIRECTORIES (. .. ${VLT_SOURCE_DIR}/implement)

set (tests
  matvecTest
  svdTest
  testMath
  tMAFilter
  )

foreach (src ${tests})
  ## executable and its source
  add_executable(${src} ${src}.cc)
  ## libraries against which to link
  target_link_libraries(${src}
    aomath
    )
endforeach (src)

## ------------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
