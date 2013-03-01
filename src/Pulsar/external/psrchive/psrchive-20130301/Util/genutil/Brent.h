//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Brent.h,v $
   $Revision: 1.5 $
   $Date: 2008/11/07 22:14:59 $
   $Author: straten $ */

#ifndef __BrentMethod
#define __BrentMethod

#include "Error.h"
#include <algorithm>
#include <math.h>

#define EPS 3.0e-8

template<class T, class U>
  T sign (T a, U b)
{
  if (b >= 0)
    return fabs(a);
  else
    return -fabs(a);
}

static unsigned iter_max = 100;

template<typename T, class Unary>
  T Brent (Unary func, T x1, T x2, T tol, T root = 0)
{
  T a = x1;
  T b = x2;
  T c = x2;
    
  T fa = func(a) - root;
  T fb = func(b) - root;

  if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0)) {
    Error error (InvalidParam, "Brent", "Root must be bracketed:");
    error << " f(" << x1 << ")=" << fa << " f(" << x2 << ")=" << fb;
    throw error;
  }

  T fc = fb;

  for (unsigned iter=0; iter<iter_max; iter++) {

    T d=0, e=0;

    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
      c=a;
      fc=fa;
      e=d=b-a;
    }
    if (fabs(fc) < fabs(fb)) {
      a=b;
      b=c;
      c=a;
      fa=fb;
      fb=fc;
      fc=fa;
    }
    T tol1 = 2.0*EPS*fabs(b) + 0.5*tol;
    T xm = 0.5*(c-b);
    if (fabs(xm) <= tol1 || fb == 0.0) {
      // std::cerr << "iter=" << iter << std::endl;
      return b;
    }
    if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) {
      T p, q, r, s=fb/fa;
      if (a == c) {
	p=2.0*xm*s;
	q=1.0-s;
      } else {
	q=fa/fc;
	r=fb/fc;
	p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
	q=(q-1.0)*(r-1.0)*(s-1.0);
      }

      if (p > 0.0) q = -q;
      p=fabs(p);
      T min1=3.0*xm*q-fabs(tol1*q);
      T min2=fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2)) {
	e=d;
	d=p/q;
      } else {
	d=xm;
	e=d;
      }
    } else {
      d=xm;
      e=d;
    }
    a=b;
    fa=fb;
    if (fabs(d) > tol1)
      b += d;
    else
      b += sign(tol1,xm);
    fb = func(b) - root;
  }

  throw Error (InvalidState, "Brent", "maximum iterations exceeded");
}

/* Minimization (as opposed to root finding) with Brent's method.
 * Set func_sign=-1 for maximization.  Returns x value for which the
 * function is minimized.
 */
#define CG (0.3819660)
#define ZEPS 1e-10
template<typename T, class Unary>
  T Brent_min (Unary func, T x1, T x2, T x3, T tol, int func_sign=1)
{
  T a = x1;
  T b = x3;
  T u=x2, v=x2, w=x2, x=x2;
  T d = 0.0, e=0.0;

  T fu, fv, fw, fx;
  fx = ((T)func_sign) * func(x);
  fv = fx;
  fw = fx;

  /* TODO check for actual bracket */

  for (unsigned ii=0; ii<iter_max; ii++) {
    T xm = 0.5 * (a+b);
    T tol1 = tol * fabs(x) + ZEPS;
    T tol2 = 2.0 * tol1;

    if (fabs(x-xm) <= (tol2-0.5*(b-a))) 
      return x;

    if (fabs(e) > tol1) {
      T r = (x-w)*(fx-fv);
      T q = (x-v)*(fx-fw);
      T p = (x-v)*q - (x-w)*r;
      q = 2.0 * (q-r);
      if (q>0.0) p = -p;
      q = fabs(q);
      T etemp=e;
      e = d;
      if (fabs(p) >= fabs(0.5*q*etemp) || p<=q*(a-x) || p>=q*(b-x)) {
        e = x>=xm ? a-x : b-x;
        d = CG * e;
      } else {
        d = p/q;
        u = x+d;
        if (u-a < tol2 || b-u < tol2)
          d = sign(tol1,xm-x);
      }
    } else {
      e = x>=xm ? a-x : b-x;
      d = CG * e;
    }

    u = fabs(d)>=tol1 ? x+d : x+sign(tol1,d);
    fu = ((T)func_sign) * func(u);

    if (fu<=fx) {
      if (u>=x) a=x;
      else b=x;
      v=w; w=x; x=u;
      fv=fw; fw=fx; fx=fu;
    } else {
      if (u<x) a=u; 
      else b=u;
      if (fu<=fw || w==x) {
        v=w; w=u; 
        fv=fw; fw=fu;
      } else if (fu<=fv || v==x || v==w) {
        v=u;
        fv=fu;
      }
    }
  }
  throw Error (InvalidState, "Brent_min", "maximum iterations exceeded");
}
#endif
