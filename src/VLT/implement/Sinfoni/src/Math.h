//#  Math.h: Some mathematical functions.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: Math.h,v 1.12 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_MATH_H
#define SINFONI_MATH_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>
#include <cmath>
#include <limits>

namespace ASTRON
{
  namespace SINFONI
  {

    // @defgroup mathematics Mathematics

    // @ingroup mathematics
    // @defgroup math_constants Math Constants
    // @{

    // Ratio of a circle's circumference to its diameter, \f$\pi\f$.
    const Real pi = M_PI; // 3.14159265358979323846264338327950288;

    // @}


    // @ingroup mathematics
    // @defgroup elementary_math Elementary Math
    // @{

    // Compare two real numbers \a x and \a y. Return true when the numbers
    // are considered equal. The two numbers are considered equal if either
    // the absolute difference of \a x and \a y is less than \a eps, or the
    // absolute ratio of the difference of \a x and \a y and the absolute
    // maximum of \a x and \a y is less than \a eps.
    inline bool compare(Real x, Real y, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      if (x == y) return true;
      if (fabs(x-y) <= eps) return true;
      if (fabs(x) > fabs(y)) return (fabs((x-y)/x) <= eps);
      else return (fabs((x-y)/y) <= eps);
    }

    // Compare two complex numbers \a x and \a y. The numbers \a x and \a y
    // are considered equal when both their real and imaginary parts compare
    // equal.
    // \see compare(Real x, Real y, Real eps)
    inline bool compare(Complex x, Complex y, 
                        Real eps = std::numeric_limits<Real>::epsilon())
    {
      return (compare(x.real(), y.real(), eps) && 
              compare(x.imag(), y.imag(), eps));
    }

    // @}



    // @defgroup signal_processing Signal Processing

    // @ingroup signal_processing
    // @defgroup waveform_generation Waveform Generation
    // @{

    // The sinc function is defined as:
    // \f[ 
    //   \mathrm{sinc}(x) = \left\{ \begin{array}{ll}
    //                                  \sin(\pi x)/(\pi x) & x \neq 0 \\
    //                                  1                   & x = 0
    //                               \end{array}
    //                       \right. 
    // \f]
    inline Real sinc(Real x)
    {
      if (x == 0.0) return 1.0;
      Real arg(pi*x);
      return sin(arg)/arg;
    }

    // @}



  } // namespace SINFONI

} // namespace ASTRON

#if defined(HAVE_BLITZ)
#include <Sinfoni/Blitz/Math.h>
#endif

#endif
