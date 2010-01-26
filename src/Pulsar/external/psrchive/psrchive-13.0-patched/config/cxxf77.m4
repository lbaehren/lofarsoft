#
# SWIN_TEST_CXXF77 ([ACTION-IF-OK [,ACTION-IF-NOT-OK]])
#
# This m4 macro tests if the F77 runtime library flags defined by FLIBS
# will cause the linker to fail on C++ code.  This is a common problem
# when both g77 and GCC version 4 is in the PATH.
#
# ----------------------------------------------------------
AC_DEFUN([SWIN_TEST_CXXF77],
[
  AC_PROVIDE([SWIN_TEST_CXXF77])
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])

  AC_MSG_CHECKING([that C++ and Fortran can be linked without conflicts])

  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $FLIBS"

  AC_LANG_PUSH(C++)

  # This breaks g++ version 4.1.1 combined with g77 version 3.4.6
  AC_TRY_LINK([#include <string>
               #include <iostream>],
              [try { std::cerr << std::endl; } catch (std::string) {}],
              can_cxxf77=yes, can_cxxf77=no)

  AC_LANG_POP(C++)

  AC_MSG_RESULT($can_cxxf77)

  LIBS="$ac_save_LIBS"

])

