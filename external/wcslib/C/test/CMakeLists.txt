
## -----------------------------------------------------------------------------
## Test programs that don't require PGPLOT.

set (wcslib_tests
  tlin
  tlog
  tprj1
  tsph
  tspx
  ttab1
  twcs
  twcssub
  tpih1
  tfitshdr
  tunits
  twcsfix
)

foreach (test ${wcslib_tests})
  ## compiler instructions
  add_executable (${test} ${test}.c)
  ## linker instructions
  target_link_libraries (${test} wcs ${libm})
  ## testing
  add_test (${test} ${test})
endforeach (test)

## Installation

install (TARGETS ${wcslib_tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## -----------------------------------------------------------------------------
## Test programs that do require PGPLOT.

set (pgplot_tests
  tspc
  tprj2
  tcel1
  tcel2
  ttab2
  ttab3
  twcsmix
)

