#
# SWIN_BEST_F77 ([ACTION-IF-OK [,ACTION-IF-NOT-OK]])
#
# This m4 macro tests if the F77 runtime library flags defined by FLIBS
# will cause the linker to fail on C++ code.  This is a common problem
# when both g77 and GCC version 4 are in the PATH.
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_BEST_F77],
[
  AC_PROVIDE([SWIN_BEST_F77])
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])

  SWIN_TEST_CXXF77

  if test x"$can_cxxf77" = xno; then

    AC_MSG_WARN([$F77 is incompatible with $CXX ... trying gfortran])

    # clear the cached value
    unset ac_cv_f77_libs

    F77=gfortran
    FLIBS=
    AC_F77_LIBRARY_LDFLAGS
    SWIN_TEST_CXXF77

    if test x"$can_cxxf77" = xno; then

AC_MSG_ERROR([

The flags required to link the Fortran runtime libraries:

FLIBS=$FLIBS

cause the linker to choose the wrong standard C++ libraries.

Please see http://psrchive.sourceforge.net/third/cxx#f77

])
    fi
  fi
])

