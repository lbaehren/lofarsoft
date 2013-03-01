//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/VonMises.h,v $
   $Revision: 1.5 $
   $Date: 2009/11/10 04:38:59 $
   $Author: straten $ */

#ifndef __VonMises_H
#define __VonMises_H

#include "MEAL/ProductRule.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"
#include "MEAL/ScalarMath.h"

namespace MEAL
{

  //! Von Mises function 
  class VonMises : public Univariate<Scalar> 
  {
  public:

    //! Default constructor
    VonMises ();

    //! Copy constructor
    VonMises (const VonMises& copy);

    //! Assignment operator
    VonMises& operator = (const VonMises& copy);

    //! Destructor
    ~VonMises ();

    //! Clone operator
    VonMises* clone () const { return new VonMises(*this); }

    //! Set the centre
    void set_centre (double centre);

    //! Get the centre
    double get_centre () const;

    //! Set the concentration
    void set_concentration (double concentration);

    //! Get the concentration
    double get_concentration () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Get the name of the class
    std::string get_name() const;

    //! Parses the values of model parameters and fit flags from a string
    void parse (const std::string& text);
  
  protected:

    void calculate (double&, std::vector<double>*) {}
    void init ();

    Reference::To<Scalar> expression;

    ScalarParameter centre;
    ScalarParameter concentration;

  };

}

#endif
