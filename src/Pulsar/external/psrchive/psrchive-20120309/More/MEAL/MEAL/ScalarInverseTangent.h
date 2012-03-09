//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarInverseTangent.h,v $
   $Revision: 1.1 $
   $Date: 2009/06/11 06:39:43 $
   $Author: straten $ */

#ifndef __MEAL_ScalarInverseTangent_H
#define __MEAL_ScalarInverseTangent_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The inverse tangent of the ratio of two Scalar functions
  class ScalarInverseTangent : public UnaryScalar
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
