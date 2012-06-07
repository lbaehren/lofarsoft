//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Value.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Complex2Value_H
#define __Complex2Value_H

#include "MEAL/Complex2.h"

namespace MEAL {

  //! Represents a complex 2x2 matrix value, \f$ J \f$
  class Complex2Value : public Complex2 {

  public:

    //! Default constructor
    Complex2Value (const Jones<double>& value = 1.0);

    //! Set the value
    void set_value (const Jones<double>& value);

    //! Get the value
    Jones<double> get_value () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (Jones<double>& x, std::vector< Jones<double> >* grad=0);

    //! The value
    Jones<double> value;

  };

}

#endif
