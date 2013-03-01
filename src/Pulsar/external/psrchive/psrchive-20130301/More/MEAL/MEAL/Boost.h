//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Boost.h,v $
   $Revision: 1.8 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_Boost_H
#define __MEAL_Boost_H

#include "MEAL/Complex2.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a boost (Hermitian, dichroic) transformation
  /*! This class represents the boost, \f$\beta\f$, along an arbitrary
    axix, \f$\hat m\f$. */
  class Boost : public Complex2 {

  public:

    Boost ();

    //! Get the unit-vector along which the boost occurs
    Vector<3, double> get_axis () const;

    //! Get the boost parameter, beta
    double get_beta () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

  };

}

#endif
