#
# SWIN_LIB_QTDRIV([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
#
# This m4 macro checks availability of the Qt device handler in the 
# PGPLOT Graphics Subroutine Library 
#
# QTDRIV_CFLAGS - autoconfig variable with flags required for compiling
# QTDRIV_LIBS   - autoconfig variable with flags required for linking
# HAVE_QTDRIV   - automake conditional
# HAVE_QTDRIV   - pre-processor macro in config.h
#
# This macro tries to link a test program using
#
#    -lqpgplot $QT_LIBS $PGPLOT_LIBS
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_QTDRIV],
[
  AC_PROVIDE([SWIN_LIB_QTDRIV])

  AC_REQUIRE([SWIN_LIB_PGPLOT])
  AC_REQUIRE([BNV_HAVE_QT])
  AC_REQUIRE([AC_F77_WRAPPERS])

  QTDRIV_CFLAGS="$QT_CXXFLAGS $PGPLOT_CFLAGS"
  QTDRIV_LIBS="-lqpgplot $QT_LIBS $PGPLOT_LIBS"

  if test $have_pgplot = yes -a $have_qt = yes; then

    AC_MSG_CHECKING([for Qt device handler in PGPLOT library])

    ac_save_LIBS="$LIBS"
    ac_save_CXXFLAGS="$CXXFLAGS"

    LIBS="$ac_save_LIBS $PGPLOT_LIBS"

    AC_TRY_LINK([char F77_FUNC(qtdriv,QTDRIV)();],[F77_FUNC(qtdriv,QTDRIV)();],
                have_qtdriv=yes, have_qtdriv=no)

    AC_MSG_RESULT($have_qtdriv)

    if test $have_qtdriv = yes; then

      AC_MSG_CHECKING([for Qt/PGPLOT library])

      AC_LANG_PUSH(C++)

      CXXFLAGS="$ac_save_CXXFLAGS $QTDRIV_CFLAGS"
      LIBS="$ac_save_LIBS $QTDRIV_LIBS"

      AC_TRY_LINK([#include "qpgplot.h"],[QPgplot widget;],
                  have_qtdriv=yes, have_qtdriv=no)

      AC_LANG_POP(C++)

      AC_MSG_RESULT($have_qtdriv)

    fi

    LIBS="$ac_save_LIBS"
    CXXFLAGS="$ac_save_CXXFLAGS"

  fi

  if test x"$have_qtdriv" = xyes; then
    AC_DEFINE([HAVE_QTDRIV], [1], [Define if PGPLOT library has Qt driver])
    [$1]
  else
    AC_MSG_WARN([PSRCHIVE rhythm will not be compiled])
    QTDRIV_CFLAGS=""
    QTDRIV_LIBS=""
    [$2]
  fi

  AC_SUBST(QTDRIV_CFLAGS)
  AC_SUBST(QTDRIV_LIBS)
  AM_CONDITIONAL(HAVE_QTDRIV, [test x"$have_qtdriv" = xyes])

])

