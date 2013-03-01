//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryScalar.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_UnaryScalar_H
#define __MEAL_UnaryScalar_H

#include "MEAL/UnaryRule.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! Abstract base class of unary functions of another Scalar function
  /*! The partial derivatives are calculated using the chain rule. */
  class UnaryScalar : public UnaryRule<Scalar>
  {

  public:

    //! The function
    virtual double function (double x) const = 0;

    //! And its first derivative
    virtual double dfdx (double x) const = 0;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the result and its gradient
    void calculate (double& result, std::vector<double>* gradient);

  };

}

#endif
