# $Id$

foreach(prog bdsm noise or_gsm)
  add_executable(${prog} ${prog}.f)
  target_link_libraries(${prog} fbdsm_io)
endforeach(prog)
