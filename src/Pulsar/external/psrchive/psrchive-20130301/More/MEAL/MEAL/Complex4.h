//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex4.h,v $
   $Revision: 1.1 $
   $Date: 2010/01/20 00:52:22 $
   $Author: straten $ */

#ifndef __MEAL_Complex4_H
#define __MEAL_Complex4_H

#include "MEAL/Evaluable.h"
#include "Matrix.h"

template<typename T>
struct EstimateTraits< Matrix< 4,4, std::complex<T> > >
{
  typedef Matrix<4,4, std::complex< Estimate<T> > > type;
};

template<typename T>
void add_variance (Matrix< 4,4, std::complex<Estimate<T> > >& result,
		   double var, const Matrix< 4,4, std::complex< T > >& grad)
{
  for (unsigned i=0; i < 4; i++)
    for (unsigned j=0; j < 4; j++)
      add_variance (result[i][j], var, grad[i][j]);
}

namespace MEAL {

  //! Pure virtual base class of all complex-valued 4x4 matrix functions
  class Complex4 : public Evaluable< Matrix< 4,4, std::complex<double> > > 
  {
  public:

    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Complex4* clone () const;
  };

}

#endif
