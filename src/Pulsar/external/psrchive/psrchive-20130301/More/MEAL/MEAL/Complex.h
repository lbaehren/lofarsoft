//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/06 11:07:20 $
   $Author: straten $ */

#ifndef __MEAL_Complex_H
#define __MEAL_Complex_H

#include "MEAL/Evaluable.h"
#include <complex>

template<typename T>
struct EstimateTraits< std::complex<T> >
{
  typedef std::complex< Estimate<T> > type;
};

template<typename T>
void add_variance (std::complex< Estimate<T> >& result,
		   double var, const std::complex<T>& grad)
{
  double re = grad.real();
  double im = grad.imag();

  std::complex< Estimate<double> > add ( Estimate<double>(0.0,re*re*var),
					 Estimate<double>(0.0,im*im*var) );
  result += add;
}

namespace MEAL
{
  //! Pure virtual base class of all complex-valued functions
  class Complex : public Evaluable< std::complex<double> >
  {
  public:
    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Complex* clone () const;
  };
}

#endif
