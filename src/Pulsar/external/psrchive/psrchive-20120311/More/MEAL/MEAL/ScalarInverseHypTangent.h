//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarInverseHypTangent.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_ScalarInverseHypTangent_H
#define __MEAL_ScalarInverseHypTangent_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The inverse hyperbolic tangent of a Scalar function
  class ScalarInverseHypTangent : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double x) const;

    //! And its first derivative
    double dfdx (double x) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  };

}


#endif
