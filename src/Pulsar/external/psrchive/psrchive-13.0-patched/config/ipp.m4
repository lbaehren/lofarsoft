dnl @synopsis SWIN_LIB_IPP
dnl 
AC_DEFUN([SWIN_LIB_IPP],
[
  AC_PROVIDE([SWIN_LIB_IPP])

  SWIN_PACKAGE_OPTIONS([ipp])

  AC_MSG_CHECKING([for IPP installation])

  if test "$have_ipp" != "user disabled"; then

    SWIN_PACKAGE_FIND([ipp],[ipps.h])
    SWIN_PACKAGE_TRY_COMPILE([ipp],[#include <ipps.h>])

    SWIN_PACKAGE_FIND([ipp],[libippsem64t.so])
    SWIN_PACKAGE_TRY_LINK([ipp],[#include <ipps.h>],
                          [ippsMalloc_32f(1);],
			  [-lippsem64t -lippcoreem64t -lpthread])


  fi

  AC_MSG_RESULT([$have_ipp])

  if test x"$have_ipp" = xyes; then
    AC_DEFINE(HAVE_IPP,1,[Define if IPP library is installed])
    [$1]
  else
    :
    [$2]
  fi

  IPP_LIBS="$ipp_LIBS"
  IPP_CFLAGS="$ipp_CFLAGS"

  AC_SUBST(IPP_CFLAGS)
  AC_SUBST(IPP_LIBS)

  AM_CONDITIONAL(HAVE_IPP,[test x"$have_ipp" = xyes])

])

