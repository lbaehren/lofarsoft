AC_DEFUN([SWIN_LIB_CULA],
[
  AC_PROVIDE([SWIN_LIB_CULA])

  AC_MSG_CHECKING([for CULA installation])

  AC_ARG_WITH([cula-dir],
              AC_HELP_STRING([--with-cula-dir=DIR],
                             [CULA is in DIR]))

  if test x"$with_cula_dir" = x"no"; then

    # user disabled cula. Leave cache alone.
    have_cula="user disabled"

  else

    # "yes" is not a specification
    if test x"$with_cula_dir" = xyes; then
      with_cula_dir=""
    fi

    CULA_CFLAGS=""
    CULA_LIBS=""

    # look for package headers and libraries where specified
    if test x"$with_cula_dir" != x; then
      CULA_CFLAGS="-I$with_cula_dir/include"
      CULA_LIBS="-L$with_cula_dir/lib64"
    elif test x"$CULA_ROOT" != x; then
      CULA_CFLAGS="-I$CULA_ROOT/include"
      CULA_LIBS="-L$CULA_ROOT/lib64"
    fi

    # the expected libraries
    CULA_LIBS="$CULA_LIBS -lcula_core -lcula_lapack -lcublas -lcudart"

    AC_LANG_PUSH(C++)
    # test compilation of simple program
    ac_save_CXXFLAGS="$CXXFLAGS"
    ac_save_LIBS="$LIBS"
    LIBS="$ac_save_LIBS $CULA_LIBS"
    CXXFLAGS="$ac_save_CXXFLAGS $CULA_CFLAGS"


    AC_TRY_LINK([#include <cula.hpp>],[culaGetCudaMinimumVersion();],
                have_cula=yes, have_cula=no)
    # if 64 bit version failed, try 32 bits
    if test x"$have_cula" = xno; then
      CULA_LIBS="-L$CULA_ROOT/lib"
      CULA_LIBS="$CULA_LIBS -lcula_core -lcula_lapack -lcublas -lcudart"
      LIBS="$ac_save_LIBS $CULA_LIBS"
      AC_TRY_LINK([#include <cula.hpp>],[culaGetCudaMinimumVersion();],
		      have_cula=yes, have_cula=no)
    fi

    AC_MSG_RESULT($have_cula)

    LIBS="$ac_save_LIBS"
    CXXFLAGS="$ac_save_CXXFLAGS"
    AC_LANG_POP(C++)

  fi

  if test x"$have_cula" = xyes; then
    AC_DEFINE([HAVE_CULA], [1], [Define to 1 if you have the CULA library])
    [$1]
  else
    AC_MSG_WARN([CULA will not be used])
    CULA_CFLAGS=""
    CULA_LIBS=""
    [$2]
  fi

  AC_SUBST(CULA_CFLAGS)
  AC_SUBST(CULA_LIBS)
  AM_CONDITIONAL(HAVE_CULA, [test x"$have_cula" = xyes])

])
