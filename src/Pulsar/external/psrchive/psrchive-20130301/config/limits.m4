#
# SWIN_TEST_LIMITS
#
# This m4 macro tests if the std::numeric_limits template is available
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_TEST_LIMITS],
[
  AC_PROVIDE([SWIN_TEST_LIMITS])

  AC_MSG_CHECKING([if C++ compiler supports std::numeric_limits traits])

  AC_LANG_PUSH(C++)

  AC_TRY_LINK([#include <limits>],
              [unsigned i = std::numeric_limits<double>::digits10;],
              has_numeric_limits=yes, has_numeric_limits=no)

  AC_LANG_POP(C++)

  AC_MSG_RESULT($has_numeric_limits)

  if test x"$has_numeric_limits" = xno; then

AC_MSG_ERROR([
The C++ compiler ($CXX) does not support a required part of the standard.
])

  fi

])

