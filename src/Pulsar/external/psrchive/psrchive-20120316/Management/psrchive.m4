# SWIN_LIB_PSRCHIVE([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_PSRCHIVE],
[
  AC_PROVIDE([SWIN_LIB_PSRCHIVE])

  AC_ARG_WITH([psrchive-dir],
              AC_HELP_STRING([--with-psrchive-dir=DIR],
                             [PSRCHIVE is installed in DIR]))

  PSRCHIVE_CFLAGS=""
  PSRCHIVE_LIBS=""

  if test x"$with_psrchive_dir" = xno; then
    # user disabled psrchive. Leave cache alone.
    have_psrchive="User disabled PSRCHIVE."
  else

    # "yes" is not a specification
    if test x"$with_psrchive_dir" = xyes; then
      with_psrchive_dir=
    fi

    if test x"$with_psrchive_dir" != x; then
      psrchive_config=$with_psrchive_dir/bin/psrchive
    else
      AC_PATH_PROG(psrchive_config, psrchive, no)
    fi

    have_psrchive="not found"

    AC_MSG_CHECKING([PSRCHIVE installation])

    if test -x $psrchive_config; then

      PSRCHIVE_CFLAGS=`$psrchive_config --cflags`
      PSRCHIVE_LIBS=`$psrchive_config --libs`

      ac_save_CPPFLAGS="$CPPFLAGS"
      ac_save_LIBS="$LIBS"

      CPPFLAGS="$PSRCHIVE_CFLAGS $CPPFLAGS"
      LIBS="$PSRCHIVE_LIBS $LIBS"

      AC_LANG_PUSH(C++)
      AC_TRY_LINK([#include "Pulsar/Archive.h"], [Pulsar::Archive::load("");],
                  have_psrchive=yes, have_psrchive=no)
      AC_LANG_POP(C++)

      if test $have_psrchive = no; then
        PSRCHIVE_CFLAGS=
        PSRCHIVE_LIBS=
      fi

      LIBS="$ac_save_LIBS"
      CPPFLAGS="$ac_save_CPPFLAGS"

    fi

  fi

  AC_MSG_RESULT([$have_psrchive])

  if test "$have_psrchive" = "yes"; then
    AC_DEFINE([HAVE_PSRCHIVE],[1],[Define if the PSRCHIVE library is present])
    [$1]
  else
    if test "$have_psrchive" = "not found"; then
      echo
      AC_MSG_NOTICE([Ensure that PSRCHIVE executables are in PATH.])
      AC_MSG_NOTICE([Alternatively, use the --with-psrchive-dir option.])
      echo
    fi
    [$2]
  fi

  AC_SUBST(PSRCHIVE_LIBS)
  AC_SUBST(PSRCHIVE_CFLAGS)
  AM_CONDITIONAL(HAVE_PSRCHIVE,[test "$have_psrchive" = "yes"])

])

