AC_DEFUN([SWIN_LIB_PKG],
[
  AC_PROVIDE([SWIN_LIB_PKG])

  AC_ARG_WITH([pkg-dir],
              AC_HELP_STRING([--with-pkg-dir=DIR],
                             [pkg is in DIR]))

  if test x"$with_pkg_dir" = x"no"; then

    # user disabled pkg. Leave cache alone.
    have_pkg="user disabled"

  else

    # "yes" is not a specification
    if test x"$with_pkg_dir" = xyes; then
      with_pkg_dir=""
    fi

    PKG_CFLAGS=""
    PKG_LIBS=""

    # look for package headers and libraries where specified
    if test x"$with_pkg_dir" != x; then
      PKG_CFLAGS="-I$with_pkg_dir/include"
      PKG_LIBS="-L$with_pkg_dir/lib"
    fi

    # the expected libraries
    PKG_LIBS="$PKG_LIBS -lfoo -lbar"

    # test compilation of simple program
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    LIBS="$ac_save_LIBS $PKG_LIBS"
    CFLAGS="$ac_save_CFLAGS $PKG_CFLAGS"
    AC_TRY_LINK([#include <pkg.h>],[pkginit(); pkgclose();],
                have_pkg=yes, have_pkg=no)

    AC_MSG_RESULT($have_pkg)

    LIBS="$ac_save_LIBS"
    CFLAGS="$ac_save_CFLAGS"

  fi

  if test x"$have_pkg" = xyes; then
    AC_DEFINE([HAVE_PKG], [1], [Define to 1 if you have the PKG library])
    [$1]
  else
    AC_MSG_WARN([PKG code will not be compiled])
    PKG_CFLAGS=""
    PKG_LIBS=""
    [$2]
  fi

  AC_SUBST(PKG_CFLAGS)
  AC_SUBST(PKG_LIBS)
  AM_CONDITIONAL(HAVE_PKG, [test x"$have_pkg" = xyes])

])
