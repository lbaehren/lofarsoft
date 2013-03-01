//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnivariatePolicy.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_UnivariatePolicy_H
#define __MEAL_UnivariatePolicy_H

#include "MEAL/ArgumentPolicy.h"

namespace MEAL {

  //! A function of one scalar variable
  class UnivariatePolicy : public ArgumentPolicy {

  public:

    //! Default constructor
    UnivariatePolicy (Function* context);

    //! Copy constructor
    UnivariatePolicy (const UnivariatePolicy&);

    //! Desctructor
    ~UnivariatePolicy ();

    //! Assignment operator
    const UnivariatePolicy& operator = (const UnivariatePolicy&);

    //! Clone operator
    UnivariatePolicy* clone (Function* context) const;

    //! Set the abscissa value
    void set_abscissa (double value);

    //! Get the abscissa value
    double get_abscissa () const;

    //! If Argument is an Axis<double>, connect it to set_abscissa
    void set_argument (unsigned dimension, Argument* axis);

  protected:

    //! The abscissa value
    double abscissa;

  };

}

#endif

