PROJECT (CASA_scimath_scimath_f C Fortran)

set (scimath_f_sources
  ADDGRD.f
  abshis.f
  absmax.f
  atmroutines.f
  convolvegridder.f
  dfftpak.f
  dqags.f
  fftpak.f
  fgridft.f
  fgridsd.f
  fmosft.f
  fwproj.f
  getbig.f
  grd2d.f
  grd2dwts.f
  grdde2d.f
  grdsf.f
  hclean.f
  lawson.f
  maxabs.f
  parametricsolver.f
  pgcrvl.f
  pgsbox.f
  phasol.f
  subcom.f
  vvroutines.f
  )

## ------------------------------------------------------------------------------
## Optimized compile commands are:
##
##  /usr/bin/g77  -fPIC -O2   -fPIC -c ???.f
##
##  /usr/bin/gcc  -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL
##                -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED   -DAIPS_NO_TEMPLATE_SRC
##                -I/opt/casa/current/code/scimath/fortran -O3 -fno-strength-reduce
##                -fPIC -DAIPS_SVID3 -DHAVE_LINUX_GLIBC -Wall -c ???.c
##

set_source_files_properties (${scimath_f_sources}
  PROPERTIES COMPILE_FLAGS "-fPIC -O2 -fPIC")

add_library (scimath_f
  ${scimath_f_sources} cpgsbox.c
  )
