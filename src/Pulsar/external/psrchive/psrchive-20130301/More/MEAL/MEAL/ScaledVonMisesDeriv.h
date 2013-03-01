//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScaledVonMisesDeriv.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __ScaledVonMisesDeriv_H
#define __ScaledVonMisesDeriv_H

#include "MEAL/ProductRule.h"
//#include "MEAL/ArgumentBehaviour.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"
#include "MEAL/ScalarMath.h"

namespace MEAL {

  //! ScaledVonMisesDeriv function 
  class ScaledVonMisesDeriv :
      public Univariate<Scalar> {

  public:

    ScaledVonMisesDeriv ();
    ScaledVonMisesDeriv (const ScaledVonMisesDeriv& copy);
    ScaledVonMisesDeriv & operator = (const ScaledVonMisesDeriv& copy);

    //! Set the centre
    void set_centre (const Estimate<double> &centre);

    //! Get the centre
    Estimate<double> get_centre () const;

    //! Set the concentration
    void set_concentration (const Estimate<double> &concentration);

    //! Get the concentration
    Estimate<double> get_concentration () const;

    //! Set the height
    void set_height (const Estimate<double> &height);

    //! Get the height 
    Estimate<double> get_height () const; 
 
    //! Connect the set_abscissa method to the axis value
    //   void set_argument (unsigned dimension, Argument* axis);

    std::string get_name() const;

    //! Parses the values of model parameters and fit flags from a string
    //    void parse (const std::string& text);
 
  protected:
    ScalarParameter height;
    ScalarParameter centre;
    ScalarParameter concentration;
    void init ();
    Reference::To<Scalar> expression;
    void calculate (double&, std::vector<double>*) {}

  };

}

#endif
