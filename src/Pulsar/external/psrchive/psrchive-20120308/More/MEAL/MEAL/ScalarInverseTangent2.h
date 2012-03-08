//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarInverseTangent2.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_ScalarInverseTangent2_H
#define __MEAL_ScalarInverseTangent2_H

#include "MEAL/BinaryScalar.h"

namespace MEAL {

  //! The inverse tangent of the ratio of two Scalar functions
  class ScalarInverseTangent2 : public BinaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // BinaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double arg1, double arg2) const;

    //! The partial derivative with respect to arg1
    double partial_arg1 (double arg1, double arg2) const;

    //! The partial derivative with respect to arg2
    double partial_arg2 (double arg1, double arg2) const;

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
