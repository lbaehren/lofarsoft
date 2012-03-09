/***************************************************************************
 *
 *   Copyright (C) 2006 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/PowerLaw.h"

using namespace std;

MEAL::PowerLaw::PowerLaw ()
  : parameters (this, 3)
{
  parameters.set_name (0, "scale");
  parameters.set_name (1, "offset");
  parameters.set_name (2, "power");
}

//! Return the name of the class
string MEAL::PowerLaw::get_name () const
{
  return "PowerLaw";
}


//! Set the scale factor
void MEAL::PowerLaw::set_scale (double scale)
{
  set_param (0, scale);
}

//! Get the scale
double MEAL::PowerLaw::get_scale () const
{
  return get_param (0);
}

//! Set the offset
void MEAL::PowerLaw::set_offset (double offset)
{
  set_param (1, offset);
}

//! Get the offset
double MEAL::PowerLaw::get_offset () const
{
  return get_param (1);
}

//! Set the power
void MEAL::PowerLaw::set_power (double power)
{
  set_param (2, power);
}

//! Get the power
double MEAL::PowerLaw::get_power () const
{
  return get_param (2);
}

//! Return the value (and gradient, if requested) of the function
void MEAL::PowerLaw::calculate (double& result, std::vector<double>* grad)
{
  double scale    = get_scale ();
  double offset   = get_offset ();
  double power    = get_power ();
  double abscissa = get_abscissa ();

  result = 0;

  if (grad) {
    grad->resize (3);
    for (unsigned ig=0; ig<3; ig++)
      (*grad)[ig] = 0;
  }

  result = (scale * pow(abscissa, power)) + offset;

  if (grad) {
    
    // dy/dscale
    (*grad)[0] = pow(abscissa, power);
    
    // dy/doffset
    (*grad)[1] = 1.0;
    
    // dy/dpower
    (*grad)[2] = scale * pow(abscissa, power) * log(abscissa);
    
  }
  
  if (verbose) {
    cerr << "MEAL::PowerLaw::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "MEAL::PowerLaw::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

