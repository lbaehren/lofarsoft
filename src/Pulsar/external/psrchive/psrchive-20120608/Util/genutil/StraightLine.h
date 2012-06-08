//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/StraightLine.h,v $
   $Revision: 1.2 $
   $Date: 2008/01/18 04:11:58 $
   $Author: straten $ */

#ifndef __StraightLine_h
#define __StraightLine_h

#include "Estimate.h"

//! Linear least squares fit to a straight line
/*!
  Implements the solution from section 15.2 of Numerical Recipes
*/
template<typename T, typename U=T>
class StraightLine
{
public:

  //! Default constructor
  StraightLine () { S = S_x = S_y = S_xx = S_xy = 0; }

  //! Addition operator
  void add_coordinate (double x, const Estimate<T,U>& d)
  {
    T y = d.get_value();
    U w = 1.0/d.get_variance();
    S += w;
    S_x += x * w;
    S_y += y * w;
    S_xx += x*x * w;
    S_xy += x*y * w;
  }

  //! Return the intercept, a
  Estimate<T,U> get_intercept () const
  {
    T delta = S * S_xx - S_x * S_x;
    T slope = (S_xx * S_y - S_x * S_xy) / delta;
    T var = S_xx / delta;
    return Estimate<T,U> (slope, var);
  }

  //! Return the slope, b
  Estimate<T,U> get_slope () const
  {
    T delta = S * S_xx - S_x * S_x;
    T intercept = (S * S_xy - S_x * S_y) / delta;
    T var = S / delta;
    return Estimate<T,U> (intercept, var);
  }

protected:
  T S, S_x, S_y, S_xx, S_xy;
};

#endif

