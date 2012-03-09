dnl @synopsis SWIN_COMPLEX_TEMPLATE
dnl
dnl If the complex template can handle an arbitrary arithmetic type without
dnl casting to standard types; then define HAVE_COMPLEX_TEMPLATE.
dnl
dnl @category Cxx
dnl @author Willem van Straten <straten@astron.nl>
dnl @version 2005-02-12
dnl @license AllPermissive

AC_DEFUN([SWIN_COMPLEX_TEMPLATE],
[AC_CACHE_CHECK(for good complex<T> template implementation,
ac_cv_cxx_complex_template,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#include<complex>

class X {
public:
 X(double x=0);
 X& operator /= (const X&);

 friend X operator * (const X&, const X&);
 friend X operator + (const X&, const X&);
 friend X operator - (const X&, const X&);
 friend X operator / (const X&, const X&);

 friend bool operator == (const X&, const X&);
 friend bool operator < (const X&, const X&);
 friend bool operator > (const X&, const X&);

 friend X operator - (const X&);
 friend X abs (const X&);
 friend X sqrt (const X&);
};
],[
std::complex<X> x;
std::complex<X> z = sqrt(x);
],
 ac_cv_cxx_complex_template=yes,
 ac_cv_cxx_complex_template=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_complex_template" = yes; then
  AC_DEFINE(HAVE_COMPLEX_TEMPLATE,1,[define if complex template is good])
fi
])

