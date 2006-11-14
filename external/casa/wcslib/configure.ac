#-----------------------------------------------------------------------------
# Process this file with autoconf-2.53 or later to produce a configure script.
#-----------------------------------------------------------------------------
# Author: Mark Calabretta, Australia Telescope National Facility
# http://www.atnf.csiro.au/~mcalabre/index.html
# $Id: configure.ac,v 4.2 2005/09/23 04:55:25 cal103 Exp $
#-----------------------------------------------------------------------------

AC_INIT([WCSLIB], [4.2], [mcalabre@atnf.csiro.au], [wcslib-4.2])
AC_PREREQ([2.53])
AC_REVISION([$Revision: 4.2 $])

AC_CONFIG_SRCDIR([C/wcs.h])
AC_CONFIG_AUX_DIR([config])

# Look for an ANSI C compiler.
AC_PROG_CC
AC_PROG_CC_STDC
AC_C_CONST
AC_TYPE_SIZE_T
if test "x$ac_cv_prog_cc_stdc" = xno -o \
        "x$ac_cv_c_const"      = xno -o \
        "x$ac_cv_type_size_t"  = xno; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    An ANSI standard C library is required to build WCSLIB.

    ERROR: WCSLIB configuration failure.
    -------------------------------------------------------], [1])

fi

# Check for ANSI C headers.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h limits.h math.h stdio.h stdlib.h string.h])
if test "x$ac_cv_header_stdc" = xno; then
  AC_MSG_ERROR([
    -------------------------------------------------------------------
    An ANSI standard C library is required to build WCSLIB.  One of the
    ANSI C header files it requires is missing or unusable.

    ERROR: WCSLIB configuration failure.
    -------------------------------------------------------------------], [1])
fi

# Checks for ANSI C library functions (suggested by autoscan).
AC_CHECK_LIB([m], [floor])
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memset pow sqrt strchr strstr])

# Check the size and availability of integer data types.
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([int *])
AC_CHECK_SIZEOF([long int])
AC_CHECK_SIZEOF([long long int])

# If sizeof(int) != sizeof(int *) we are in serious trouble.
if test "x$ac_cv_sizeof_int" != "x$ac_cv_sizeof_int"; then
  AC_MSG_WARN([
    ---------------------------------------------------------------------
    WARNING: WCSLIB data structures require sizeof(int) == sizeof(int *);
             structs may be misaligned.
    ---------------------------------------------------------------------])
fi

if test "x$ac_cv_sizeof_int" = x8; then
AC_DEFINE([WCS_INT64], [int], [64-bit integer data type.])
elif test "x$ac_cv_sizeof_long_int" = x8; then
AC_DEFINE([WCS_INT64], [long int], [64-bit integer data type.])
elif test "x$ac_cv_sizeof_long_long_int" = x8; then
AC_DEFINE([WCS_INT64], [long long int], [64-bit integer data type.])
fi

# Utilities.
AC_PROG_RANLIB
AC_PROG_LN_S
AC_PROG_INSTALL

# Look for a Fortran compiler.
AC_PROG_F77
if test "x$F77" = x; then
  AC_MSG_WARN([
    ---------------------------------------------------------------------
    Fortran compiler not found, will skip PGSBOX and Fortran test suite.
    ---------------------------------------------------------------------])
else
  if test "x$ac_cv_f77_compiler_gnu" = xyes ; then
    FFLAGS="$FFLAGS -Wno-globals"
  fi

  AC_MSG_CHECKING(whether $F77 accepts -I)
  AC_LANG_PUSH(Fortran 77)
  FFLAGS_save=$FFLAGS
  FFLAGS=-I.
  AC_TRY_COMPILE([], [], [FFLAGS="$FFLAGS_save -I."; AC_MSG_RESULT(yes)],
    [FFLAGS="$FFLAGS_save"; AC_MSG_RESULT(no)])
  AC_LANG_POP(Fortran 77)
fi


# The following are required for building the test suite.
# -------------------------------------------------------
AC_MSG_NOTICE([

Looking for libraries etc. required to build the test suite...])

# Check for other standard header files.
AC_CHECK_HEADERS([unistd.h])


# Extra places to look for third-party include files and libraries.
INCDIRS="/usr/local/include \
         /usr/local/pgplot  \
         /usr/local/cfitsio \
         /local/pgplot      \
         /local/cfitsio"
LIBDIRS="/usr/local/lib     \
         /usr/local/pgplot  \
         /usr/local/cfitsio \
         /local/lib         \
         /local/pgplot      \
         /local/cfitsio     \
         /opt/SUNWspro/lib"

for LIBDIR in $LIBDIRS ; do
  AC_CHECK_FILE([$LIBDIR], [LDFLAGS="$LDFLAGS -L$LIBDIR"], [continue])
done


# Search for PGPLOT.
for INCDIR in /usr/include $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/cpgplot.h], [PGPLOTINC=-I$INCDIR; break])
  INCDIR=$INCDIR/pgplot
  AC_CHECK_FILE([$INCDIR/cpgplot.h], [PGPLOTINC=-I$INCDIR; break])
done

# Only helps if the libraries were built using the same compiler(s).
#AC_F77_LIBRARY_LDFLAGS

# PGPLOT compiled with a SUN compiler but linked with something else.
AC_CHECK_LIB([sunmath], [cosd],      [PGPLOTLIB="-lsunmath $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])
AC_CHECK_LIB([M77],     [iand_],     [PGPLOTLIB="-lM77 $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])
AC_CHECK_LIB([F77],     [f77_init],  [PGPLOTLIB="-lF77 $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])

# PGPLOT compiled with a GNU compiler but linked with something else.
AC_CHECK_LIB([g2c],     [gerror_],   [PGPLOTLIB="-lg2c $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])

# Search for X11 includes and libraries.
AC_PATH_X
if test "x$ac_x_libraries" != x; then
  LDFLAGS="$LDFLAGS -L$ac_x_libraries"
  PGPLOTLIB="-lX11 $PGPLOTLIB"
fi

# It is possible that other libraries may be required depending on what
# graphics drivers were installed with PGPLOT.
AC_CHECK_LIB([z],       [deflate],   [PGPLOTLIB="-lz $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])
AC_CHECK_LIB([png],     [png_error], [PGPLOTLIB="-lpng $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])
AC_CHECK_LIB([pgplot],  [pgbeg_],    [PGPLOTLIB="-lpgplot $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])
AC_CHECK_LIB([cpgplot], [cpgbeg],    [PGPLOTLIB="-lcpgplot $PGPLOTLIB"],
             [PGPLOTLIB=], [$PGPLOTLIB $LIBS])

if test "x$PGPLOTLIB" = x; then
  AC_MSG_WARN([PGPLOT not found, skipping PGPLOT-dependent tests.])
else
  AC_MSG_NOTICE([PGPLOT appears to be available.])
  AC_DEFINE([HAVE_PGPLOT], [1], [Define to 1 if PGPLOT is available.])
fi

AC_SUBST([PGPLOTINC])
AC_SUBST([PGPLOTLIB])


# Search for CFITSIO.
for INCDIR in /usr/include $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
  INCDIR=$INCDIR/cfitsio
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
done

AC_CHECK_LIB([socket],  [recv], [CFITSIOLIB="-lsocket"], [], [$LIBS])
AC_CHECK_LIB([cfitsio], [ffopen],
             [CFITSIOLIB="-lcfitsio $CFITSIOLIB"; break], [],
             [$CFITSIOLIB $LIBS])

if test "x$CFITSIOINC" = x -o "x$CFITSIOLIB" = x; then
  AC_MSG_WARN([CFITSIO not found, skipping CFITSIO-dependent tests.])
else
  AC_MSG_NOTICE([CFITSIO appears to be available.])
  AC_DEFINE([HAVE_CFITSIO], [1], [Define to 1 if CFITSIO is available.])
fi

AC_SUBST([CFITSIOINC])
AC_SUBST([CFITSIOLIB])


AC_CONFIG_FILES([makedefs])
AC_CONFIG_HEADER([C/config.h])
AC_OUTPUT
