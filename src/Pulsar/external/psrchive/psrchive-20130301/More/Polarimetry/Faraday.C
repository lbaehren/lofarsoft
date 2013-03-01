/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Faraday.h"
#include "MEAL/OneParameter.h"
#include "Physical.h"
#include "Pauli.h"

using namespace std;

// #define _DEBUG 1

Calibration::Faraday::Faraday () :
  rotation (Pauli::basis().get_basis_vector(2))
{
  MEAL::OneParameter* parm = new MEAL::OneParameter (this);
  parm->set_name (0, "RM");
  parm->set_description (0, "rotation measure (rad/m^2)");

  reference_wavelength = 0.0;
  wavelength = 0.0;
}

//! Return the name of the class
string Calibration::Faraday::get_name () const
{
  return "Faraday";
}

//! Set the rotation measure
void Calibration::Faraday::set_rotation_measure (const Estimate<double>& rm)
{
  set_Estimate (0, rm);
}

//! Get the rotation measure
Estimate<double> Calibration::Faraday::get_rotation_measure () const
{
  return get_Estimate (0);
}

/*! This alias enables use of this class with the ColdPlasma template */
void Calibration::Faraday::set_measure (double rm)
{
  set_param (0, rm);
}

/*! This alias enables use of this class with the ColdPlasma template */
double Calibration::Faraday::get_measure () const
{
  return get_param (0);
}

//! Set the reference frequency in MHz
void Calibration::Faraday::set_reference_frequency (double MHz)
{
  set_reference_wavelength( Pulsar::speed_of_light / (MHz * 1e6) );
}

//! Get the reference frequency in MHz
double Calibration::Faraday::get_reference_frequency () const
{
  return 1e-6 * Pulsar::speed_of_light / reference_wavelength;
}


//! Set the frequency in MHz
void Calibration::Faraday::set_frequency (double MHz)
{
  set_wavelength( Pulsar::speed_of_light / (MHz * 1e6) );
}

//! Get the frequency in MHz
double Calibration::Faraday::get_frequency () const
{
  return 1e-6 * Pulsar::speed_of_light / wavelength;
}

//! Set the reference wavelength in metres
void Calibration::Faraday::set_reference_wavelength (double metres)
{
  if (reference_wavelength == metres)
    return;

  reference_wavelength = metres;
  set_evaluation_changed ();
}

//! Get the reference wavelength in metres
double Calibration::Faraday::get_reference_wavelength () const
{
  return reference_wavelength;
}


//! Set the wavelength in metres
void Calibration::Faraday::set_wavelength (double metres)
{
  if (wavelength == metres)
    return;

  wavelength = metres;
  set_evaluation_changed ();
}

//! Get the wavelength in metres
double Calibration::Faraday::get_wavelength () const
{
  return wavelength;
}

void Calibration::Faraday::set_axis (const Vector<3, double>& axis)
{
  rotation.set_axis (axis);
  copy_evaluation_changed (rotation);
}

double Calibration::Faraday::get_rotation () const
{
  // The OneParameter policy stores the rotation measure
  double rotation_measure = get_param (0);

  double lambda_0 = reference_wavelength;
  double lambda = wavelength;

#ifndef _DEBUG
  if (verbose)
#endif
    cerr << "Calibration::Faraday::get_rotation lambda_0=" 
	 << reference_wavelength
	 << " lambda=" << wavelength << " (metres)" << endl;

  double rot = rotation_measure * (lambda*lambda - lambda_0*lambda_0);

#ifndef _DEBUG
  if (verbose)
#endif
    cerr << "Calibration::Faraday::get_rotation RM=" << rotation_measure
	 << " result=" << rot << " radians" << endl;

  return rot;
}

//! Calculate the Jones matrix and its gradient
void Calibration::Faraday::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  /* remember that the Rotation transformation rotates the basis, so the
     effect on the coherency matrix is negative */

  rotation.set_phi ( -get_rotation() );
  result = rotation.evaluate (grad);

#ifdef _DEBUG
  cerr << "rotation=" << rotation.get_phi() 
       << " det(J)=" << det(result) << endl;
#endif

  if (!grad)
    return;

  if (grad->size() != 1)
    throw Error (InvalidState, "Calibration::Faraday::calculate",
                 "gradient.size=%d != 1", grad->size());

  double dphi_dRM = rotation.get_phi().get_value() / get_param(0);

  (*grad)[0] *= dphi_dRM;

#ifdef _DEBUG
  cerr << " det(grad)=" << det((*grad)[0]) << endl;
#endif

}

