//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Phase.h,v $
   $Revision: 1.11 $
   $Date: 2009/06/13 05:09:24 $
   $Author: straten $ */

#ifndef __MEAL_Phase_H
#define __MEAL_Phase_H

#include "MEAL/OneParameter.h"
#include <complex>

namespace MEAL
{
  //! A complex exponential parameterized by phase
  template<typename Parent>
  class Phase : public Parent 
  {

  public:

    //! Default constructor
    Phase (double _scale = 1.0) : parameter (this)
    { parameter.set_name ("phase"); scale = _scale; }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "Phase<" + std::string(Parent::Name) + ">"; }

  protected:

    typedef typename Parent::Result Return;

    //! Calculate the result and its gradient
    void calculate (Return& result, std::vector<Return>* gradient);
   
    OneParameter parameter;
    double scale;
  };

}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
template<typename Parent>
void MEAL::Phase<Parent>::calculate (Return& result, std::vector<Return>* grad)
{
  double phase = this->get_param(0);

  double cos_phase = cos(scale * phase);
  double sin_phase = sin(scale * phase);

  if (MEAL::Function::verbose)
    std::cerr << get_name () << "::calculate scale=" << scale 
	      << " phase=" << phase*180/M_PI << " deg" 
	      << " cos=" << cos_phase << " sin=" << sin_phase << std::endl;

  result = std::complex<double>(cos_phase, sin_phase);

  if (grad)
  {
    (*grad)[0] = scale * std::complex<double>(-sin_phase, cos_phase);
    
    if (MEAL::Function::verbose)
      std::cerr << "MEAL::Phase<Parent>::calculate gradient" << std::endl
	   << "   " << (*grad)[0] << std::endl;
  } 
}

#endif
