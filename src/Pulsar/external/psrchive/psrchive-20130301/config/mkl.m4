dnl @synopsis SWIN_LIB_MKL
dnl 
AC_DEFUN([SWIN_LIB_MKL],
[
  AC_PROVIDE([SWIN_LIB_MKL])

  SWIN_PACKAGE_OPTIONS([mkl])

  AC_MSG_CHECKING([for Intel Math Kernel Library (MKL) DFTI installation])

  if test x"$MKL" == x; then
    MKL=mkl
  fi

  if test "$have_mkl" != "user disabled"; then

    SWIN_PACKAGE_FIND([mkl],[mkl_dfti.h])
    SWIN_PACKAGE_TRY_COMPILE([mkl],[#include <mkl_dfti.h>])

    SWIN_PACKAGE_FIND([mkl],[lib$MKL.*])
    SWIN_PACKAGE_TRY_LINK([mkl],[#include <mkl_dfti.h>],
                          [DftiCreateDescriptor
                           ( 0, DFTI_SINGLE, DFTI_REAL, 0, 0 );],
                          [-l$MKL -lguide -lpthread -lm])

  fi

  AC_MSG_RESULT([$have_mkl])

  if test x"$have_mkl" = xno; then

    AC_MSG_CHECKING([for old Intel Math Kernel Library (MKL) installation])
    SWIN_PACKAGE_TRY_LINK([mkl],[void cfft1d_(float*,int*,int*,float*);],
                          [cfft1d_(0,0,0,0);],
                          [-l$MKL -lguide -lpthread -lm])


    AC_MSG_RESULT([$have_mkl])

    if test x"$have_mkl" = xyes; then
      have_mkl=old
      AC_DEFINE([HAVE_MKL],[1],
                [Define if the old Intel Math Kernel Library is present])
      [$1]
    fi

  elif test x"$have_mkl" = xyes; then

    AC_DEFINE([HAVE_MKL_DFTI],[1],
              [Define if the Intel Math Kernel Library DFTI is present])
    [$1]

  else
    :
    [$2]
  fi

  MKL_LIBS="$mkl_LIBS"
  MKL_CFLAGS="$mkl_CFLAGS"

  AC_SUBST(MKL_LIBS)
  AC_SUBST(MKL_CFLAGS)
  AM_CONDITIONAL(HAVE_MKL_DFTI,[test "$have_mkl" = yes])
  AM_CONDITIONAL(HAVE_MKL,[test "$have_mkl" = old])

])

