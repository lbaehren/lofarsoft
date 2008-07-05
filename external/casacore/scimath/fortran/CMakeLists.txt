
project (casacore_scimath_f C Fortran)

## -----------------------------------------------------------------------------
## libscimath_f
##
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

add_library (casa_scimath_f
  ${scimath_f_sources}
  ${LAPACK_LIBRARIES}
  )

set_target_properties (casa_scimath_f
  PROPERTIES
  COMPILE_FLAGS "-fPIC -O2 -fPIC"
  LINKER_LANGUAGE Fortran
  )

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS casa_scimath_f
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
