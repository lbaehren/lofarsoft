/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Simulation.h"
#include "Pulsar/SingleAxis.h"

#include "Pauli.h"

using namespace std;

bool Calibration::Simulation::verbose = false;

Calibration::Simulation::Simulation ()
{
  response_changed = true;

  cal_U = .95;
  cal_V = .0;

}

//! Set Stokes V of the reference signal
void Calibration::Simulation::set_cal_V (double val)
{
  cal_V = val;
}

//! Set Stokes U of the reference signal
void Calibration::Simulation::set_cal_U (double val)
{
  cal_U = val;
}


//! Return the model of the instrument
Calibration::Instrument* Calibration::Simulation::get_instrument ()
{
  response_changed = true;
  return &instrument;
}

Jones<double> Calibration::Simulation::get_response ()
{
  if (response_changed) {

    response = instrument.evaluate();

    // not concerned with gain transformations
    response /= sqrt(det(response));

    inverse = inv(response);

  }

  response_changed = false;

  return response;
}

void Calibration::Simulation::single_axis_calibrate ()
{
  // the input calibrator state
  Stokes<double> calibrator (1, 0, cal_U, cal_V);

  response = instrument.evaluate();
  response /= sqrt(det(response));

  Stokes<double> calibrator_obs = transform( calibrator, response );

  if (verbose)
    cerr << "Calibration::Simulation::single_axis_calibrate observed ="
	 << calibrator_obs << endl;

  // construct the coherencies of the observed calibrator
  vector<Estimate<double> > calibrator_coh (4);

  calibrator_coh[0] = 0.5 * (calibrator_obs[0] + calibrator_obs[1]);
  calibrator_coh[1] = 0.5 * (calibrator_obs[0] - calibrator_obs[1]);
  calibrator_coh[2] = 0.5 * calibrator_obs[2];
  calibrator_coh[3] = 0.5 * calibrator_obs[3];

  // fake variances for now
  for (unsigned ipol=0; ipol<4; ipol++)
    calibrator_coh[ipol].var = 1e-4;

  // solve
  Calibration::SingleAxis backend;
  backend.solve (calibrator_coh);

  if (verbose)
    cerr << "Calibration::Simulation::single_axis_calibrate\n"
      "   differential gain  = " << backend.get_diff_gain().val << endl <<
      "   differential phase = " << backend.get_diff_phase().val * 180/M_PI
	 << endl;

  Jones<double> calibrator_jones = backend.evaluate();

  // not concerned with gain transformations
  calibrator_jones /= sqrt(det(calibrator_jones));

  // compute the transformation experienced by the data

  response = inv(calibrator_jones) * response;
  inverse = inv(response);

  if (!verbose)
    return;

  // polar decompose this transformation

  Quaternion<double, Hermitian> H;
  Quaternion<double, Unitary> U;
  complex<double> d;
  
  polar (d, H, U, response);

  cerr << "Calibration::Simulation::single_axis_calibrate det=" << d << endl;

  // calculate the boost impact parameter

  Vector<3,double> boost = H.get_vector ();
  double sinhbeta = sqrt (boost * boost);
  double beta = asinh (sinhbeta);

  cerr << "\nUnmodeled boost=" << H << endl;
  cerr << "beta=" << beta << endl;

  double predicted_I_distortion = exp(2*beta) - 1;
  cerr << "\n Theoretical maximum distortion to Stokes I = " 
       << predicted_I_distortion << endl;

  // calculate the rotation angle

  Vector<3,double> rotation = U.get_vector ();
  double sinphi = sqrt (rotation * rotation);
  double phi = atan2 (sinphi, U[0]);

  cerr << "\nUnmodeled rotation=" << U << endl;
  cerr << "phi=" << phi*180/M_PI << " deg" << endl << endl;

}

//! Set the input source state
void Calibration::Simulation::set_input (Stokes<double>& st)
{
  input = st;

  if (response_changed)
    get_response();

  output = transform( input, response );

  input_set = true;
}

//! Set the output source state
void Calibration::Simulation::set_output (Stokes<double>& st)
{
  output = st;

  if (response_changed)
    get_response();

  input = transform( output, inverse );

  input_set = false;
}

//! Get the distortion to this parameter
double Calibration::Simulation::get_distortion (Measurable m)
{
  if (input_set)
    return get_distortion (m, input, output);
  else
    return get_distortion (m, output, input);
}

//! Get the distortion to this parameter
double Calibration::Simulation::get_distortion (Measurable m,
						Stokes<double>& in,
						Stokes<double>& out)
{
  double L_out = sqrt(out[1]*out[1] + out[2]*out[2]);
  double L_in = sqrt(in[1]*in[1] + in[2]*in[2]);

  switch (m) {
	
  case StokesI:
    return out[0] - in[0];

  case StokesQ:
    return out[1] - in[1];

  case StokesU:
    return out[2] - in[2];

  case StokesV:
    return out[3] - in[3];
    
  case StokesP: {
    Vector<3,double> diff = out.get_vector() - in.get_vector();
    return sqrt(diff*diff);
  }
  
  case StokesL:
    return L_out - L_in;

  case Orientation: {
    // Project the observed polarization vector into the Q,U plane
    Vector<3,double> vect_out = out.get_vector();
    vect_out[2] = 0.0;
    // normalize it
    if (L_out > 0.0)
      vect_out /= L_out;
    
    // Project the observed polarization vector into the Q,U plane
    Vector<3,double> vect_in = in.get_vector();
    vect_in[2] = 0.0;
    // normalize it
    if (L_in > 0.0)
      vect_in /= L_in;
    
    Vector<3,double> sT = cross (vect_in, vect_out);
    
    double cos_dTheta = vect_out * vect_in;
    double sin_dTheta = sT*sT;
    
    if (sT[2] < 0.0)
      sin_dTheta *= -1.0;
    
    return .5 * atan2(sin_dTheta, cos_dTheta);
  }
  
  case Ellipticity: {
    double Epsilon_out = .5 * atan2(out[3], L_out);
    double Epsilon = .5 * atan2(in[3], L_in);
    return Epsilon_out - Epsilon;
  }
  
  }

  return 0.0;
}
