dnl @synopsis SWIN_PROG_FIND
dnl
dnl Ensures that the find program is the one that searches for files in a
dnl directory hierarchy.
dnl 
AC_DEFUN([SWIN_PROG_FIND],
[
  AC_PROVIDE([SWIN_PROG_FIND])
  AC_REQUIRE([AC_PROG_FGREP])

  AC_MSG_CHECKING([that find is not SIGPROC find])

  sigproc_find=`find 2>/dev/null | $FGREP SIGPROC`
  if test x"$sigproc_find" = x; then
    AC_MSG_RESULT(ok)
  else
    AC_MSG_ERROR([Please ensure that SIGPROC find is not in PATH])
  fi

])

