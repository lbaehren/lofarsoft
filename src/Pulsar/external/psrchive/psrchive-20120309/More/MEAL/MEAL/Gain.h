//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gain.h,v $
   $Revision: 1.10 $
   $Date: 2009/06/10 10:53:53 $
   $Author: straten $ */

#ifndef __MEAL_Gain_H
#define __MEAL_Gain_H

#include "MEAL/OneParameter.h"

namespace MEAL
{

  //! A gain transformation
  template<typename Parent>
  class Gain : public Parent
  {

  public:

    //! Default constructor
    Gain ();

    //! Set the gain
    void set_gain (const Estimate<double>& gain)
    { this->set_Estimate (0, gain); }

    //! Get the gain
    Estimate<double> get_gain () const
    { return this->get_Estimate (0); }

    //! Set the name of the parameter
    void set_param_name (const std::string& name)
    { parameter.set_name (name); }

    //! Set the description of the parameter
    void set_param_description (const std::string& name)
    { parameter.set_description (name); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "Gain<" + std::string(Parent::Name) + ">"; }

  protected:

    typedef typename Parent::Result Return;

    //! Calculate the result and its gradient
    void calculate (Return& result, std::vector<Return>* gradient);

    OneParameter parameter;
  };

}

template<typename Parent>
MEAL::Gain<Parent>::Gain () : parameter (this)
{
  parameter.set_name ("gain");
  parameter.set_param (1.0);
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
template<typename Parent>
void MEAL::Gain<Parent>::calculate (Return& result, std::vector<Return>* grad)
{
  result = this->get_param(0);

  if (MEAL::Function::verbose)
    std::cerr << "MEAL::Gain::calculate gain=" << result << std::endl;

  if (grad)
  {
    (*grad)[0] = 1.0;
    
    if (MEAL::Function::verbose)
      std::cerr << "MEAL::Gain::calculate gradient" << std::endl
	   << "   " << (*grad)[0] << std::endl;
  }
  
}

#endif
