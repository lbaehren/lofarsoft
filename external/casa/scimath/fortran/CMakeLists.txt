
project (CASA_scimath_libscimath C Fortran)

## -----------------------------------------------------------------------------
## libscimath_f
##
##  /usr/bin/g77  -fPIC -O2   -fPIC -c ???.f
##
##  /usr/bin/gcc  -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL
##                -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED   -DAIPS_NO_TEMPLATE_SRC
##                -I/opt/casa/current/code/scimath/fortran -O3 -fno-strength-reduce
##                -fPIC -DAIPS_SVID3 -DHAVE_LINUX_GLIBC -Wall -c ???.c

## collect the source files in the modules

FILE (GLOB scimath_f_sources *.f *.c)

## build the library

set_source_files_properties (${scimath_f_sources}
  PROPERTIES COMPILE_FLAGS "-fPIC -O2 -fPIC")

add_library (scimath_f
  ${scimath_f_sources}
  )
