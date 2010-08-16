#
# SWIN_LIB_PUMA([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
#
# This m4 macro checks availability of the PUMA Library
#
# PUMA_CFLAGS - autoconfig variable with flags required for compiling
# PUMA_LIBS   - autoconfig variable with flags required for linking
# HAVE_PUMA   - automake conditional
# HAVE_PUMA   - pre-processor macro in config.h
#
# This macro tries to link a test program, using
#
#    -L$DEVEL_INSTALL/lib -lpuma -lputils
#
# Notice that the environment variable DEVEL_INSTALL is used.
#
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_PUMA],
[
  AC_PROVIDE([SWIN_LIB_PUMA])

  AC_MSG_CHECKING([for PUMA libary installation])

  PUMA_CFLAGS="-I$DEVEL_INSTALL/include"
  PUMA_LIBS="-L$DEVEL_INSTALL/lib -lpuma -lputils"

  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $PUMA_LIBS"
  CFLAGS="$ac_save_CFLAGS $PUMA_CFLAGS"

  AC_TRY_LINK([#include <libpuma.h>],[prheader(0,0);],
              have_puma=yes, have_puma=no)

  AC_MSG_RESULT($have_puma)

  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"

  if test x"$have_puma" = xyes; then
    AC_DEFINE([HAVE_PUMA], [1], [Define to 1 if you have the PUMA library])
    [$1]
  else
    AC_MSG_WARN([PUMA file format will not be supported])
    if test x"$DEVEL_INSTALL" = x; then
      AC_MSG_WARN([Please set the DEVEL_INSTALL environment variable])
    fi
    PUMA_CFLAGS=""
    PUMA_LIBS=""
    [$2]
  fi

  AC_SUBST(PUMA_CFLAGS)
  AC_SUBST(PUMA_LIBS)
  AM_CONDITIONAL(HAVE_PUMA, [test x"$have_puma" = xyes])

])

