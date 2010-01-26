#
# SWIN_LIB_READLINE([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
#
# This m4 macro checks availability of the GNU readline library
#
# READLINE_CFLAGS - autoconf variable with flags required for compiling
# READLINE_LIBS   - autoconf variable with flags required for linking
# HAVE_READLINE   - automake conditional
# HAVE_READLINE   - pre-processor macro in config.h
#
# This macro tries to link a test program using
#
#    -lreadline -ltermcap
#
#
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_READLINE],
[
  AC_PROVIDE([SWIN_LIB_READLINE])

  AC_MSG_CHECKING([for GNU readline installation])

  READLINE_CFLAGS=""
  READLINE_LIBS="-lreadline -ltermcap"

  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $READLINE_LIBS"
  CFLAGS="$ac_save_CFLAGS $READLINE_CFLAGS"

  AC_TRY_LINK([#include <stdio.h>
	       #include <readline/readline.h>
               #include <readline/history.h>],
              [readline(0); rl_completion_matches(0,0);],
              have_readline=yes, have_readline=no)

  AC_MSG_RESULT($have_readline)

  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"

  if test x"$have_readline" = xyes; then
    AC_DEFINE([HAVE_READLINE], [1], [Define to 1 if GNU readline is installed])
    [$1]
  else
    AC_MSG_WARN([GNU readline will not be used])
    READLINE_CFLAGS=""
    READLINE_LIBS=""
    [$2]
  fi

  AC_SUBST(READLINE_CFLAGS)
  AC_SUBST(READLINE_LIBS)
  AM_CONDITIONAL(HAVE_READLINE, [test x"$have_readline" = xyes])

])

