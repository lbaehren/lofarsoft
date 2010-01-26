#
# SWIN_LIB_GSL([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
#
# This m4 macro checks availability of the GSL Library
#
# GSL_CFLAGS - autoconfig variable with flags required for compiling
# GSL_LIBS   - autoconfig variable with flags required for linking
# HAVE_GSL   - automake conditional
# HAVE_GSL   - pre-processor macro in config.h
#
# This macro tries to get GSL cflags and libs using the
# gsl-config program.  If that is not available, it 
# will try to link using:
#
#    -lgsl -lgslcblas -lm
#
# NOTE: this is not the gsl.m4 script that comes with the
# GSL source distribution.
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_GSL],
[
  AC_PROVIDE([SWIN_LIB_GSL])

  AC_MSG_CHECKING([for GSL libary installation])

  AC_PATH_PROG(GSL_CONFIG, gsl-config, no)
  if test "$GSL_CONFIG" = "no" ; then 
    GSL_CFLAGS=""
    GSL_LIBS="-lgsl -lgslcblas -lm"
  else
    GSL_CFLAGS=`$GSL_CONFIG --cflags`
    GSL_LIBS=`$GSL_CONFIG --libs`
  fi 

  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $GSL_LIBS"
  CFLAGS="$ac_save_CFLAGS $GSL_CFLAGS"

  AC_TRY_LINK([#include <gsl/gsl_blas.h>],[gsl_blas_dgemm(0,0,0,0,0,0,0);],
              have_gsl=yes, have_gsl=no)

  AC_MSG_RESULT($have_gsl)

  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"

  if test x"$have_gsl" = xyes; then
    AC_DEFINE([HAVE_GSL], [1], [Define to 1 if you have the GSL library])
    [$1]
  else
    AC_MSG_WARN([GSL-dependent code will not be compiled.])
    GSL_CFLAGS=""
    GSL_LIBS=""
    [$2]
  fi

  AC_SUBST(GSL_CFLAGS)
  AC_SUBST(GSL_LIBS)
  AM_CONDITIONAL(HAVE_GSL, [test x"$have_gsl" = xyes])

])

