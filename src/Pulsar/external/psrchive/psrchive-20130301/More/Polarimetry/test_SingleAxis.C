/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/SingleAxis.h"
#include "Pulsar/SingleAxisSolver.h"
#include "Pauli.h"

using namespace std;

template <class T>
void random_gain (complex<T>& val)
{
  T mod, phi;
  random_value (mod, 1.0);
  random_value (phi, 2.0*M_PI);

  mod += 10.0;

  val = polar (mod, phi);
}

int main () try {

  // MEAL::Function::verbose = true;
  srand (13);

  // more complex solution
  Calibration::SingleAxisSolver solver;

  // the model of the receiver
  Calibration::SingleAxis model;

  // calibrator input
  Stokes<double> cal (1,0,1,0);

  // coherency products
  vector<Estimate<double> > cal_hi (4);

  unsigned nloop = unsigned(20000);

  cerr << "Generating " << nloop << " random source and receiver combinations"
       << endl;

  for (unsigned i=0; i<nloop; i++)  {

    // switch basis for each test
    if (i%2) {
      Pauli::basis().set_basis (Signal::Circular);
      cal = Stokes<double> (1,1,0,0);
    }
    else {
      Pauli::basis().set_basis (Signal::Linear);
      cal = Stokes<double> (1,0,1,0);
    }

    // randomly generated Stokes parameters
    Stokes<double> source;
    random_value (source, 10.0, 1.0);

    // randomly generated feed gains
    complex<double> gain_x;
    complex<double> gain_y;
    random_gain (gain_x);
    random_gain (gain_y);

    // cerr << "gain_x=" << gain_x << " gain_y=" << gain_y << endl;

    // gain
    complex<double> Gain = sqrt (gain_x * gain_y);

    // differential gain and phase
    complex<double> diff = gain_x / gain_y;

    // cerr << "norm(diff)=" << norm(diff) << endl;

    double G = sqrt(norm(Gain));
    double beta = 0.25 * log (norm(diff));
    double phi = 0.5 * arg(diff);

    // cerr << "G=" << G << " beta=" << beta << " phi=" << phi << endl;

    Jones<double> jones (gain_x, 0,
			 0, gain_y);

    // calculate the  Stokes parameters of the transformed source
    Stokes<double> image = transform( source, jones );

    // test two different methods
    unsigned istart = 0;

    for (unsigned itest=istart; itest < 2; itest++) {

      if (itest == 0) {
	
	// solve for the system, assuming CAL = [1,0,1,0]

	// calculate the transformed calibrator coherencies
	Jones<double> coherencies = jones * convert(cal) * herm (jones);

	cal_hi[0].val = coherencies.j00.real();
	cal_hi[1].val = coherencies.j11.real();
	cal_hi[2].val = coherencies.j10.real();
	cal_hi[3].val = coherencies.j10.imag();
	
	model.solve (cal_hi);

      }

      else {

	// calculate the Stokes parameters of the transformed source
	Stokes< Estimate<double> > cal_out = transform( cal, jones );

	solver.set_input (cal);
	solver.set_output (cal_out);
	solver.solve (&model);

      }

      if (fabs((model.get_gain().val - G)/G) > 1e-5)
	cerr << "model.gain=" << model.get_gain().val
	     << " != gain="<< G <<endl;
      
      if (fabs((model.get_diff_gain().val - beta)/beta) > 1e-5)
	cerr << "model.boost=" << model.get_diff_gain().val 
	     << " != boost="<< beta <<endl;
      
      if (fabs((model.get_diff_phase().val - phi)/phi) > 1e-5)
	cerr << "model.phase=" << model.get_diff_phase().val 
	     << " != phase="<< phi <<endl;
      
      // calibrate the image Stokes vector
      
      // inverse of system response
      Jones<double> response = model.evaluate ();
      
      complex<double> abs_phase = polar (1.0, arg(Gain));
      
      response *= abs_phase;
      
      double diffnorm = norm (response - jones);
      if (diffnorm > 1e-10) {
	response *= -1.0;
	diffnorm = norm (response - jones);
      }
      
      if (diffnorm > 1e-10) {
	cerr << "\ntest " << itest
	     << "\noutput response=" << response << " != "
	     << "\ninput  response=" << jones << " diff=" << diffnorm << endl;
	return -1;
      }
      
      Jones<double> calibrator = inv (response);
      
      Stokes<double> calibrated = transform (image, calibrator);

      if (norm (calibrated - source) / norm(source) > 1e-10)
	cerr << "calibrated=" << image << " !=\nsource=" << source << endl;

    }

  }

  cerr << "SingleAxis and SingleAxisSolver pass in Linear and Circular bases"
       << endl;

  return 0;
}
catch (Error& error) {
 cerr << error << endl;
 return -1;
}

