# SWIN_PSRCAT([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_PSRCAT],
[
  AC_PROVIDE([SWIN_PSRCAT])

  AC_ARG_WITH([psrcat],
              AC_HELP_STRING([--with-psrcat=PROG],
                             [where PROG is 'psrcat' or 'psrinfo']))

  AC_MSG_CHECKING([for PSRCAT installation])

  case "$with_psrcat" in
  psrcat)
    have_psrcat="psrcat"
  ;;
  psrinfo)
    have_psrcat="psrinfo"
  ;;
  *)
    have_psrcat="testit"
  esac

  if test $have_psrcat = "testit"; then

    # Test if psrcat can be executed

    psrcat_version=`psrcat -v 2> /dev/null`
    case "$psrcat_version" in
      *"Catalogue version number"*)
        have_psrcat="psrcat"
      ;;
    esac

  fi

  if test $have_psrcat = "testit"; then

    # Test if psrinfo can be executed

    psrcat_version=`psrinfo -V 2> /dev/null`
    case "$psrcat_version" in
      *psrinfo.c*)
        have_psrcat="psrinfo"
      ;;
    esac

  fi

  if test $have_psrcat = "testit"; then
    have_psrcat="not found"
  fi

  AC_MSG_RESULT([$have_psrcat])

  if test "$have_psrcat" = "psrcat"; then
    AC_DEFINE([HAVE_PSRCAT], [1], [Define if PSRCAT is installed])
    [$1]
  else
    if test "$have_psrcat" = "not found"; then
      echo
      AC_MSG_NOTICE([Ensure that psrcat or psrinfo is in PATH.])
      AC_MSG_NOTICE([Alternatively, use the --with-psrcat option.])
      echo
    fi
    [$2]
  fi

  AM_CONDITIONAL(HAVE_PSRCAT,[test "$have_psrcat" = "psrcat"])

])

