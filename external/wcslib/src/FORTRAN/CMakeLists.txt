
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
  add_executable (${test} ${test}_f.c)
endforeach (test)