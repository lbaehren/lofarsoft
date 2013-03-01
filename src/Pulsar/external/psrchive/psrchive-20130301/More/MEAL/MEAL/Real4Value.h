//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Real4Value.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Real4Value_H
#define __Real4Value_H

#include "MEAL/Real4.h"

namespace MEAL {

  //! Represents a complex 2x2 matrix value, \f$ J \f$
  class Real4Value : public Real4 {

  public:

    //! Default constructor
    Real4Value ();

    //! Set the value
    void set_value (const Matrix<4,4,double>& value);

    //! Get the value
    Matrix<4,4,double> get_value () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (Matrix<4,4,double>& x, 
		    std::vector< Matrix<4,4,double> >* grad=0);

    //! The value
    Matrix<4,4,double> value;

  };

}

#endif
