/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SingleAxis.h"

#include "MEAL/Gain.h"
#include "MEAL/Boost1.h"
#include "MEAL/Rotation1.h"
#include "MEAL/CyclicParameter.h"

#include "Pauli.h"
#include "Error.h"

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;

void Calibration::SingleAxis::init ()
{
  gain = new MEAL::Gain<MEAL::Complex2>;
  gain->set_param_name ("G");
  gain->set_param_description ("scalar gain");
  add_model (gain);

  boost = new MEAL::Boost1    (Vector<3, double>::basis(0));
  boost->set_param_name ("gamma");
  boost->set_param_description ("differential gain (hyperbolic radians)");
  add_model (boost);

  rotation = new MEAL::Rotation1 (Vector<3, double>::basis(0));
  rotation->set_param_name ("phi");
  rotation->set_param_description ("differential phase (radians)");
  add_model (rotation);
}

Calibration::SingleAxis::SingleAxis ()
{
  init ();
}

Calibration::SingleAxis::~SingleAxis ()
{
}

Calibration::SingleAxis::SingleAxis (const SingleAxis& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const Calibration::SingleAxis& 
Calibration::SingleAxis::operator = (const SingleAxis& s)
{
  if (&s == this)
    return *this;

  *gain = *(s.gain);
  *boost = *(s.boost);
  *rotation = *(s.rotation);

  return *this;
}

Calibration::SingleAxis* Calibration::SingleAxis::clone () const
{
  return new SingleAxis (*this);
}

//! Return the name of the class
string Calibration::SingleAxis::get_name () const
{
  return "SingleAxis";
}

//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> Calibration::SingleAxis::get_gain () const
{
  return gain->get_Estimate(0);
}

//! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
Estimate<double> Calibration::SingleAxis::get_diff_gain () const
{
  return boost->get_Estimate(0);
}

//! Get the differential phase, \f$ \phi \f$, in radians
Estimate<double> Calibration::SingleAxis::get_diff_phase () const
{
  return rotation->get_Estimate(0);
}

void check (const char* name, const Estimate<double>& g)
{
  if (!finite (g.val))
    throw Error (InvalidParam, name, "Estimate value = %lf", g.val);
  if (!finite (g.var))
    throw Error (InvalidParam, name, "Estimate variance = %lf", g.var);
}

//! Set the instrumental gain, \f$ G \f$, in calibrator flux units
void Calibration::SingleAxis::set_gain (const Estimate<double>& g)
{
  check ("Calibration::SingleAxis::set_gain", g);
  gain->set_Estimate (0, g);
}

//! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
void Calibration::SingleAxis::set_diff_gain (const Estimate<double>& gamma)
{
  check ("Calibration::SingleAxis::set_diff_gain", gamma);
  boost->set_Estimate (0, gamma);
}
   
//! Set the differential phase, \f$ \phi \f$, in radians
void Calibration::SingleAxis::set_diff_phase (const Estimate<double>& phi)
{
  check ("Calibration::SingleAxis::set_diff_phase", phi);
  rotation->set_Estimate (0, phi);
}

//! Set the axis along which the boost and rotation occur
void Calibration::SingleAxis::set_axis (const Vector<3, double>& axis)
{
  boost->set_axis (axis);
  rotation->set_axis (axis);
}

//! Get the unit-vector along which the boost and rotation occur
Vector<3, double> Calibration::SingleAxis::get_axis () const
{
  return boost->get_axis ();
}

void Calibration::SingleAxis::set_cyclic (bool flag)
{
  const MEAL::OneParameter* current = 
    dynamic_cast<const MEAL::OneParameter*> (rotation->get_parameter_policy());

  if (!current)
    throw Error (InvalidState, "Calibration::SingleAxis::set_cyclic",
		 "Rotation1 parameter policy is not a OneParameter");

  if (flag)
  {
    // set up the cyclic boundary for orientation
    MEAL::CyclicParameter* cyclic = 0;
    cyclic = new MEAL::CyclicParameter (*current);
    rotation->set_parameter_policy (cyclic);

    cyclic->set_period (M_PI);
    cyclic->set_upper_bound (M_PI/2);
    cyclic->set_lower_bound (-M_PI/2); 
  }
  else
  {
    MEAL::OneParameter* noncyclic = 0;
    noncyclic = new MEAL::OneParameter (*current);
    rotation->set_parameter_policy (noncyclic);
  }
}


/*! Given the output coherency products (and cross products) of the
  input linear calibrator, S=[I=1,Q=0,U=1,V=0], this method solves for
  the instrumental gain, differential gain (and differential phase). */
void Calibration::SingleAxis::solve (const vector<Estimate<double> >& cal)
{
  if (verbose)
    cerr << "Calibration::SingleAxis::solve" << endl;

  unsigned npol = cal.size();

  for (unsigned ipol=0; ipol<npol; ++ipol)
  {
    bool invalid = false;

    if (ipol < 2 && cal[ipol].val <= 0)
    {
      if (verbose)
	cerr << "Calibration::SingleAxis::solve invalid flux[" << ipol << "]="
	     << cal[ipol].val << endl;
      invalid = true;
    }

    if (cal[ipol].var <= 0)
    {
      if (verbose)
	cerr << "Calibration::SingleAxis::solve invalid var[" << ipol << "]="
	     << cal[ipol].var << endl;
      if (check_variance)
	invalid = true;
    }

    if (invalid)
    {
      set_gain (0);
      set_diff_gain (0);
      set_diff_phase (0);
      return;
    }
  }

  if (npol == 1)
  {
    set_gain ( sqrt(cal[0]) );
    set_diff_gain (0);
    set_diff_phase (0);
    return;
  }

  Estimate<double> cal_AA = cal[0];
  Estimate<double> cal_BB = cal[1];

  // note: there is an error in Britton 2000, following Equation (14):
  // correction: \beta = 1/2 ln (g_a/g_b)
  
  // note also: that this equation refers to the Jones matrix and, since
  // we are dealing with detected coherency products:
  //   cal_AA = g_a^2 C/2
  //   cal_BB = g_b^2 C/2
  // where C is the cal flux in each probe (assumed equal in this model)
  
  set_diff_gain( 0.25 * log (cal_AA / cal_BB) );
    
  // gain, G = g_a g_b C;
  Estimate<double> G = 2.0 * sqrt (cal_AA * cal_BB);

  set_gain( sqrt(G) );
  
  if (verbose)
    cerr << "Calibration::SingleAxis::solve gamma=" << get_diff_gain()
	 << " gain=" << get_gain() << endl;

  if (npol == 2)
  {
    set_diff_phase (0);
    return;
  }

  if (npol != 4)
    throw Error (InvalidParam, "Calibration::SingleAxis::solve",
		 "invalid npol=%d", npol);

  Estimate<double> cal_ReAB = cal[2];
  Estimate<double> cal_ImAB = cal[3];

  if (cal_ImAB == 0 && cal_ReAB == 0)
  {
    if (verbose)
      cerr << "Calibration::SingleAxis::solve invalid phase" << endl;
    set_diff_phase (0);
    return;
  }
  
  set_diff_phase( -0.5 * atan2 (cal_ImAB, cal_ReAB) );
  
  if (verbose)
    cerr << "Calibration::SingleAxis::solve phase=" << get_diff_phase() <<endl;
}


void Calibration::SingleAxis::invert ()
{
  if (verbose) cerr << "Calibration::SingleAxis::invert\n"
     "  gain=" << get_gain() << "\n"
     "  gamma=" << get_diff_gain() << "\n"
     "  phi=" << get_diff_phase() << endl;

  set_gain       ( 1.0 / get_gain() );
  set_diff_gain  ( -get_diff_gain() );
  set_diff_phase ( -get_diff_phase() );
}

const Calibration::SingleAxis& 
Calibration::SingleAxis::operator *= (const SingleAxis& other)
{
  if (verbose) cerr << "Calibration::SingleAxis::operator *=\n"
		 "  this.gain=" << get_gain() << 
		 " that.gain=" << other.get_gain() << "\n"
		 "  this.gama=" << get_diff_gain() <<
		 " that.gama=" << other.get_diff_gain()<< "\n"
		 "  this. phi=" << get_diff_phase() <<
		 " that.phi=" << other.get_diff_phase() << "\n";

  set_gain       ( get_gain()  * other.get_gain() );
  set_diff_gain  ( get_diff_gain() + other.get_diff_gain() );
  set_diff_phase ( get_diff_phase()   + other.get_diff_phase() );

  return *this;
}

//! Get the gain transformation
MEAL::Gain<MEAL::Complex2>* Calibration::SingleAxis::get_gain_transformation ()
{
  return gain;
}

    //! Get the boost transformation
MEAL::Boost1* Calibration::SingleAxis::get_boost_transformation ()
{
  return boost;
}

//! Get the rotation transformation
MEAL::Rotation1* Calibration::SingleAxis::get_rotation_transformation ()
{
  return rotation;
}
