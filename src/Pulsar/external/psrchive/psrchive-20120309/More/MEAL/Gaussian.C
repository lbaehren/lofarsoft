/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Gaussian.h"

using namespace std;

MEAL::Gaussian::Gaussian ()
  : parameters (this, 3)
{
  period = 0.0;

  parameters.set_name (0, "centre");
  parameters.set_name (1, "width");
  parameters.set_name (2, "height");
}

//! Return the name of the class
string MEAL::Gaussian::get_name () const
{
  return "Gaussian";
}


//! Set the centre
void MEAL::Gaussian::set_centre (double centre)
{
  set_param (0, centre);
}

//! Get the centre
double MEAL::Gaussian::get_centre () const
{
  return get_param (0);
}

//! Set the width
void MEAL::Gaussian::set_width (double width)
{
  set_param (1, width);
}

//! Get the width
double MEAL::Gaussian::get_width () const
{
  return get_param (1);
}

//! Set the height
void MEAL::Gaussian::set_height (double height)
{
  set_param (2, height);
}

//! Get the height
double MEAL::Gaussian::get_height () const
{
  return get_param (2);
}

//! Set the period
void MEAL::Gaussian::set_period (double _period)
{
  period = _period;
}

//! Get the period
double MEAL::Gaussian::get_period () const
{
  return period;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::Gaussian::calculate (double& result, std::vector<double>* grad)
{
  double centre   = get_centre ();
  double width    = get_width ();
  double height   = get_height ();
  double abscissa = get_abscissa ();

  double current_centre = centre;

  result = 0;

  if (grad) {
    grad->resize (3);
    for (unsigned ig=0; ig<3; ig++)
      (*grad)[ig] = 0;
  }

  int iter = 1;

  if (period) {
    current_centre -= period;
    iter = 3;
  }

  for (int i=0; i<iter; i++) {

    double dist = abscissa - current_centre;
    double arg = dist / width;

    double ex = exp (-arg*arg);
    double fac = height * ex * 2.0 * arg / width;
    
    result += height * ex;

    if (grad) {

      // dy/dcentre
      (*grad)[0] += fac;

      // dy/dwidth
      (*grad)[1] += fac * arg;
      
      // dy/dheight
      (*grad)[2] += ex;
    
    }

    current_centre += period;
    
  }

  if (verbose) {
    cerr << "MEAL::Gaussian::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "MEAL::Gaussian::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

