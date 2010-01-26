dnl @synopsis ATNF_DISABLE_OPTIMIZE
dnl
AC_DEFUN([ATNF_DISABLE_OPTIMIZE],
[
	AC_PROVIDE([ANTF_DISABLE_OPTIMIZE])

	AC_ARG_ENABLE([optimize],
		AC_HELP_STRING([--disable-optimize],
			[disable optimization] ),
			[optimize=yes])

	if test x"$optimize" = xyes; then
		echo 's/-O2 //g' > .ac_strip_O2
		echo 's/-O2$//g' >> .ac_strip_O2

		if test x"$swin_cxxflags_set" != xyes; then
		CXXFLAGS=`echo $CXXFLAGS | sed -f .ac_strip_O2`
		else
		AC_MSG_NOTICE([   CXXFLAGS set by user.])
		fi
		
		if test x"$swin_cflags_set" != xyes; then
		CFLAGS=`echo $CFLAGS | sed -f .ac_strip_O2`
		else
		AC_MSG_NOTICE([   CFLAGS set by user.])
		fi
		
		if test x"$swin_fflags_set" != xyes; then
		FFLAGS=`echo $FFLAGS | sed -f .ac_strip_O2`
		else
		AC_MSG_NOTICE([   FFLAGS set by user.])
		fi
		
		rm -f .ac_strip_O2
	fi
		
	
])
