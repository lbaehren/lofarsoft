//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2.h,v $
   $Revision: 1.8 $
   $Date: 2009/06/06 11:07:20 $
   $Author: straten $ */

#ifndef __MEAL_Complex2_H
#define __MEAL_Complex2_H

#include "MEAL/Complex.h"
#include "Jones.h"

template<typename T>
struct EstimateTraits< Jones<T> >
{
  typedef Jones< Estimate<T> > type;
};

template<typename T>
void add_variance (Jones< Estimate<T> >& result,
		   double var, const Jones<T>& grad)
{
  for (unsigned i=0; i < 4; i++)
    add_variance (result[i], var, grad[i]);
}

namespace MEAL
{
  //! Pure virtual base class of all complex 2x2 matrix functions
  class Complex2 : public Evaluable< Jones<double> >
  {
  public:
    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Complex2* clone () const;
  };
}

#endif
